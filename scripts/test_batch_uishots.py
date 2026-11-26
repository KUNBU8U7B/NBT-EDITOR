#!/usr/bin/env python3
"""Regression tests for the provider-safe screenshot batching workflow."""

from __future__ import annotations

import hashlib
import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
SCRIPT = PROJECT_ROOT / "scripts" / "batch_uishots.py"


class BatchUishotsTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary = tempfile.TemporaryDirectory(prefix="nbt-uishots-")
        self.root = Path(self.temporary.name)
        self.images_dir = self.root / "shots"
        self.state_path = self.root / "state" / "batches.json"
        self.images_dir.mkdir()

    def tearDown(self) -> None:
        self.temporary.cleanup()

    def make_images(self, count: int) -> list[Path]:
        paths: list[Path] = []
        for index in range(1, count + 1):
            path = self.images_dir / f"{index:02d}.png"
            path.write_bytes(f"fixture-{index}".encode("ascii"))
            paths.append(path)
        return paths

    def run_cli(self, *args: str, expect_success: bool = True) -> subprocess.CompletedProcess[str]:
        command = [
            sys.executable,
            str(SCRIPT),
            "--dir",
            str(self.images_dir),
            "--state",
            str(self.state_path),
            *args,
        ]
        result = subprocess.run(
            command,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        if expect_success and result.returncode != 0:
            self.fail(
                f"command failed ({result.returncode}): {' '.join(command)}\n"
                f"stdout:\n{result.stdout}\nstderr:\n{result.stderr}"
            )
        if not expect_success and result.returncode == 0:
            self.fail(f"command unexpectedly succeeded: {' '.join(command)}")
        return result

    def json_output(self, *args: str) -> object:
        result = self.run_cli(*args)
        try:
            return json.loads(result.stdout)
        except json.JSONDecodeError as error:
            self.fail(f"invalid JSON output: {error}\n{result.stdout}")

    def test_manifest_caps_nine_images_at_eight(self) -> None:
        original = self.make_images(9)
        manifest = self.json_output("--manifest")
        assert isinstance(manifest, dict)
        batches = manifest["batches"]
        assert isinstance(batches, list)
        self.assertEqual([batch["count"] for batch in batches], [8, 1])
        self.assertTrue(all(batch["count"] <= 8 for batch in batches))
        flattened = [
            image for batch in batches for image in batch["images"]
        ]
        self.assertEqual(
            flattened,
            [str(path.resolve()) for path in original],
        )

    def test_manifest_splits_seventeen_images_deterministically(self) -> None:
        original = self.make_images(17)
        manifest = self.json_output("--manifest")
        assert isinstance(manifest, dict)
        batches = manifest["batches"]
        assert isinstance(batches, list)
        self.assertEqual([batch["count"] for batch in batches], [8, 8, 1])
        self.assertTrue(all(batch["count"] <= 8 for batch in batches))
        flattened = [image for batch in batches for image in batch["images"]]
        self.assertEqual(flattened, [str(path.resolve()) for path in original])

    def test_failed_batch_is_the_only_batch_retried(self) -> None:
        self.make_images(17)
        self.json_output("--manifest")

        first = self.json_output("--claim")
        assert isinstance(first, dict)
        self.assertEqual(first["index"], 1)
        self.run_cli("--mark-success", "1")

        second = self.json_output("--claim")
        assert isinstance(second, dict)
        self.assertEqual(second["index"], 2)
        self.run_cli("--mark-failed", "2", "--error", "provider timeout")

        third = self.json_output("--claim")
        assert isinstance(third, dict)
        self.assertEqual(third["index"], 3)
        self.run_cli("--mark-success", "3")

        failed = self.json_output("--failed")
        assert isinstance(failed, list)
        self.assertEqual([batch["index"] for batch in failed], [2])

        retried = self.json_output("--retry")
        assert isinstance(retried, dict)
        self.assertEqual(retried["index"], 2)
        self.assertEqual(retried["attempts"], 2)
        self.run_cli("--mark-success", "2")

        failed = self.json_output("--failed")
        assert isinstance(failed, list)
        self.assertEqual(failed, [])

    def test_retry_refuses_when_no_batch_failed(self) -> None:
        self.make_images(9)
        self.json_output("--manifest")
        result = self.run_cli("--retry", expect_success=False)
        self.assertIn("no failed batch", result.stderr)

    def test_changed_screenshot_set_invalidates_stale_mark(self) -> None:
        self.make_images(9)
        self.json_output("--manifest")
        self.make_images(10)  # add a new screenshot after the state was created

        result = self.run_cli("--mark-success", "1", expect_success=False)
        self.assertIn("stale", result.stderr)

        status = self.json_output("--status")
        assert isinstance(status, dict)
        counts = status["counts"]
        assert isinstance(counts, dict)
        self.assertEqual(counts["pending"], 2)
        self.assertEqual(counts["success"], 0)

    def test_interrupted_claim_can_be_reclaimed(self) -> None:
        self.make_images(9)
        self.json_output("--manifest")
        claimed = self.json_output("--claim")
        assert isinstance(claimed, dict)
        self.assertEqual(claimed["index"], 1)

        self.run_cli("--reclaim-in-progress")
        status = self.json_output("--status")
        assert isinstance(status, dict)
        counts = status["counts"]
        assert isinstance(counts, dict)
        self.assertEqual(counts["pending"], 2)
        self.assertEqual(counts["in_progress"], 0)

    def test_concurrent_claims_select_different_batches(self) -> None:
        self.make_images(9)
        self.json_output("--manifest")
        command = [
            sys.executable,
            str(SCRIPT),
            "--dir",
            str(self.images_dir),
            "--state",
            str(self.state_path),
            "--claim",
        ]
        workers = [subprocess.Popen(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE) for _ in range(2)]
        outputs = [worker.communicate() for worker in workers]
        for worker, (stdout, stderr) in zip(workers, outputs):
            self.assertEqual(worker.returncode, 0, stderr)
        indices = [json.loads(stdout)["index"] for stdout, _ in outputs]
        self.assertEqual(sorted(indices), [1, 2])

    def test_state_cannot_be_written_inside_screenshot_directory(self) -> None:
        self.make_images(9)
        command = [
            sys.executable,
            str(SCRIPT),
            "--dir",
            str(self.images_dir),
            "--state",
            str(self.images_dir / "progress.json"),
            "--manifest",
        ]
        result = subprocess.run(
            command,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            check=False,
        )
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("outside the screenshot directory", result.stderr)
        self.assertFalse((self.images_dir / "progress.json").exists())

    def test_workflow_never_changes_screenshot_files(self) -> None:
        original = self.make_images(9)
        before = {
            path.name: hashlib.sha256(path.read_bytes()).hexdigest()
            for path in original
        }
        self.json_output("--manifest")
        self.run_cli("--claim")
        self.run_cli("--mark-success", "1")
        after = {
            path.name: hashlib.sha256(path.read_bytes()).hexdigest()
            for path in original
        }
        self.assertEqual(before, after)


if __name__ == "__main__":
    unittest.main(verbosity=2)
