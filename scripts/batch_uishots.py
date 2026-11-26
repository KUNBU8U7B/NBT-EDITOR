#!/usr/bin/env python3
"""Plan and track provider-safe UI screenshot attachment batches.

The screenshot directory is never modified, copied, renamed, or deleted.  The
only file this utility writes is a small JSON state file outside that directory;
it records batch progress so a failed batch can be retried without repeating
successful batches.  Image contents are never loaded into memory.
"""

from __future__ import annotations

import argparse
import contextlib
import fcntl
import hashlib
import json
import os
import re
import sys
import tempfile
from pathlib import Path
from typing import Iterator, Sequence

MAX_IMAGES_PER_PROMPT = 8
SUPPORTED_IMAGE_SUFFIXES = frozenset({".png", ".jpg", ".jpeg", ".gif", ".webp"})
PROJECT_ROOT = Path(__file__).resolve().parent.parent
DEFAULT_STATE_PATH = PROJECT_ROOT / "out" / "uishot-batch-state.json"
STATE_VERSION = 1

STATUS_PENDING = "pending"
STATUS_IN_PROGRESS = "in_progress"
STATUS_SUCCESS = "success"
STATUS_FAILED = "failed"


class StateError(RuntimeError):
    """Raised when persisted batch state cannot be used safely."""


def _natural_key(path: Path) -> tuple[tuple[int, object], ...]:
    """Sort numbered screenshot names naturally (2 before 10).

    Each component carries a type marker so mixed names such as ``1.png`` and
    ``a.png`` never trigger a Python int-vs-str comparison error.
    """
    return tuple(
        (0, int(part)) if part.isdigit() else (1, part.casefold())
        for part in re.split(r"(\d+)", path.name)
    )


def find_images(directory: Path) -> list[Path]:
    """Return supported image files in deterministic order."""
    if not directory.exists():
        raise FileNotFoundError(f"screenshot directory does not exist: {directory}")
    if not directory.is_dir():
        raise NotADirectoryError(f"screenshot path is not a directory: {directory}")

    return sorted(
        (
            path
            for path in directory.iterdir()
            if path.is_file() and path.suffix.casefold() in SUPPORTED_IMAGE_SUFFIXES
        ),
        key=lambda path: (_natural_key(path), path.name.casefold(), path.name),
    )


def make_batches(
    images: Sequence[Path], max_images: int = MAX_IMAGES_PER_PROMPT
) -> list[list[Path]]:
    """Split images into batches, never exceeding the requested hard limit."""
    if not 1 <= max_images <= MAX_IMAGES_PER_PROMPT:
        raise ValueError(
            f"max_images must be between 1 and {MAX_IMAGES_PER_PROMPT}"
        )
    return [
        list(images[index : index + max_images])
        for index in range(0, len(images), max_images)
    ]


def _display_path(path: Path) -> str:
    try:
        return str(path.resolve().relative_to(PROJECT_ROOT))
    except ValueError:
        return str(path.resolve())


def _manifest_fingerprint(directory: Path, images: Sequence[Path]) -> str:
    """Identify a screenshot set using names and cheap filesystem metadata.

    The files themselves are not read.  Any file addition, removal, reorder, or
    replacement with a different size/mtime invalidates old batch statuses.
    """
    digest = hashlib.sha256()
    digest.update(str(directory.resolve()).encode("utf-8", "surrogateescape"))
    digest.update(b"\0")
    for path in images:
        stat = path.stat()
        record = (
            f"{path.name}\0{stat.st_size}\0{stat.st_mtime_ns}\0"
        ).encode("utf-8", "surrogateescape")
        digest.update(record)
    return digest.hexdigest()


def _batch_records(
    batches: Sequence[Sequence[Path]],
) -> list[dict[str, object]]:
    return [
        {
            "index": index,
            "count": len(batch),
            "images": [_display_path(path) for path in batch],
        }
        for index, batch in enumerate(batches, start=1)
    ]


def _manifest(
    directory: Path,
    images: Sequence[Path],
    batches: Sequence[Sequence[Path]],
    state: dict[str, object] | None = None,
) -> str:
    payload: dict[str, object] = {
        "directory": _display_path(directory),
        "image_count": len(images),
        "max_images_per_prompt": MAX_IMAGES_PER_PROMPT,
        "batch_count": len(batches),
        "batches": _batch_records(batches),
    }
    if state is not None:
        statuses = {
            int(batch["index"]): batch
            for batch in _state_batches(state)
        }
        for batch in payload["batches"]:  # type: ignore[union-attr]
            persisted = statuses.get(int(batch["index"]))
            if persisted is not None:
                batch["status"] = persisted["status"]
                batch["attempts"] = persisted["attempts"]
                batch["error"] = persisted["error"]
    return json.dumps(payload, indent=2, ensure_ascii=False)


def _print_text(
    directory: Path, images: Sequence[Path], batches: Sequence[Sequence[Path]]
) -> None:
    print(
        f"{_display_path(directory)}: {len(images)} image(s), "
        f"{len(batches)} batch(es), max {MAX_IMAGES_PER_PROMPT} per prompt"
    )
    for index, batch in enumerate(batches, start=1):
        print(f"\nBatch {index} ({len(batch)}):")
        for path in batch:
            print(f"  {_display_path(path)}")


def _state_batches(state: dict[str, object]) -> list[dict[str, object]]:
    batches = state.get("batches")
    if not isinstance(batches, list):
        raise StateError("state file has no valid batch list")
    result: list[dict[str, object]] = []
    for batch in batches:
        if not isinstance(batch, dict):
            raise StateError("state file contains an invalid batch entry")
        result.append(batch)
    return result


def _new_state(
    directory: Path,
    images: Sequence[Path],
    batches: Sequence[Sequence[Path]],
    fingerprint: str,
) -> dict[str, object]:
    records = _batch_records(batches)
    for record in records:
        record.update(
            {
                "status": STATUS_PENDING,
                "attempts": 0,
                "error": None,
            }
        )
    return {
        "version": STATE_VERSION,
        "directory": _display_path(directory),
        "fingerprint": fingerprint,
        "image_count": len(images),
        "max_images_per_prompt": MAX_IMAGES_PER_PROMPT,
        "batch_count": len(batches),
        "batches": records,
    }


def _read_state(state_path: Path) -> dict[str, object] | None:
    if not state_path.exists():
        return None
    try:
        with state_path.open("r", encoding="utf-8") as handle:
            state = json.load(handle)
    except (OSError, json.JSONDecodeError) as error:
        raise StateError(f"cannot read state file {state_path}: {error}") from error
    if not isinstance(state, dict):
        raise StateError(f"state file is not a JSON object: {state_path}")
    if state.get("version") != STATE_VERSION:
        raise StateError(
            f"unsupported state version in {state_path}; run --reset-state"
        )
    _state_batches(state)
    return state


def _write_state(state_path: Path, state: dict[str, object]) -> None:
    """Atomically replace the state file so interrupted writes cannot corrupt it."""
    state_path.parent.mkdir(parents=True, exist_ok=True)
    payload = json.dumps(state, indent=2, ensure_ascii=False) + "\n"
    fd, temporary_name = tempfile.mkstemp(
        prefix=f".{state_path.name}.", suffix=".tmp", dir=state_path.parent
    )
    temporary = Path(temporary_name)
    try:
        with os.fdopen(fd, "w", encoding="utf-8") as handle:
            handle.write(payload)
            handle.flush()
            os.fsync(handle.fileno())
        os.replace(temporary, state_path)
        try:
            directory_fd = os.open(state_path.parent, os.O_RDONLY)
            try:
                os.fsync(directory_fd)
            finally:
                os.close(directory_fd)
        except OSError:
            # The replace is already durable on common filesystems; directory
            # fsync is best-effort on platforms that reject directory handles.
            pass
    finally:
        try:
            temporary.unlink()
        except FileNotFoundError:
            pass


@contextlib.contextmanager
def _state_lock(state_path: Path) -> Iterator[None]:
    """Serialize state transitions across concurrent screenshot workers."""
    state_path.parent.mkdir(parents=True, exist_ok=True)
    lock_path = state_path.with_name(state_path.name + ".lock")
    with lock_path.open("a+", encoding="utf-8") as lock:
        fcntl.flock(lock.fileno(), fcntl.LOCK_EX)
        try:
            yield
        finally:
            fcntl.flock(lock.fileno(), fcntl.LOCK_UN)


def _current_plan(
    directory: Path,
) -> tuple[list[Path], list[list[Path]], str]:
    images = find_images(directory)
    batches = make_batches(images)
    fingerprint = _manifest_fingerprint(directory, images)
    return images, batches, fingerprint


def _ensure_state(
    state_path: Path,
    directory: Path,
    images: Sequence[Path],
    batches: Sequence[Sequence[Path]],
    fingerprint: str,
) -> dict[str, object]:
    state = _read_state(state_path)
    if state is None or state.get("fingerprint") != fingerprint:
        state = _new_state(directory, images, batches, fingerprint)
        _write_state(state_path, state)
    return state


def _state_summary(state: dict[str, object]) -> str:
    counts = {
        STATUS_PENDING: 0,
        STATUS_IN_PROGRESS: 0,
        STATUS_SUCCESS: 0,
        STATUS_FAILED: 0,
    }
    batches: list[dict[str, object]] = []
    for batch in _state_batches(state):
        status = str(batch.get("status", STATUS_PENDING))
        if status not in counts:
            raise StateError(f"state file contains an unknown status: {status}")
        counts[status] += 1
        batches.append(
            {
                "index": batch.get("index"),
                "count": batch.get("count"),
                "status": status,
                "attempts": batch.get("attempts", 0),
                "error": batch.get("error"),
            }
        )
    return json.dumps(
        {
            "directory": state.get("directory"),
            "fingerprint": state.get("fingerprint"),
            "batch_count": state.get("batch_count"),
            "counts": counts,
            "batches": batches,
        },
        indent=2,
        ensure_ascii=False,
    )


def _find_batch(state: dict[str, object], index: int) -> dict[str, object]:
    for batch in _state_batches(state):
        if int(batch.get("index", -1)) == index:
            return batch
    raise StateError(f"batch must be between 1 and {state.get('batch_count', 0)}")


def _claim_batch(
    state: dict[str, object], wanted_status: str
) -> dict[str, object]:
    for batch in _state_batches(state):
        if batch.get("status") == wanted_status:
            batch["status"] = STATUS_IN_PROGRESS
            batch["attempts"] = int(batch.get("attempts", 0)) + 1
            return batch
    if wanted_status == STATUS_FAILED:
        raise StateError("no failed batch is available to retry")
    raise StateError("no pending batch is available; use --retry for failed batches")


def _claim_output(batch: dict[str, object]) -> str:
    return json.dumps(batch, indent=2, ensure_ascii=False)


def _resolve_state_path(value: Path) -> Path:
    path = value.expanduser()
    if not path.is_absolute():
        path = (PROJECT_ROOT / path).resolve()
    return path


def _validate_state_location(state_path: Path, directory: Path) -> None:
    """Keep progress metadata out of the immutable screenshot directory."""
    try:
        state_path.resolve().relative_to(directory.resolve())
    except ValueError:
        return
    raise StateError("state file must be outside the screenshot directory")


def _parse_args(argv: Sequence[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Plan or track UI screenshot attachment batches (the screenshot "
            f"files are read-only; maximum {MAX_IMAGES_PER_PROMPT} images per "
            "batch)."
        )
    )
    parser.add_argument(
        "--dir",
        type=Path,
        default=PROJECT_ROOT / "out" / "uishots",
        help="screenshot directory (default: out/uishots)",
    )
    parser.add_argument(
        "--state",
        type=Path,
        default=DEFAULT_STATE_PATH,
        help="progress state file (default: out/uishot-batch-state.json)",
    )

    output = parser.add_mutually_exclusive_group()
    output.add_argument(
        "--manifest",
        action="store_true",
        help="print a JSON manifest containing all batches and statuses",
    )
    output.add_argument(
        "--batch",
        type=int,
        metavar="N",
        help="print only batch N, one image path per line",
    )
    output.add_argument(
        "--claim",
        action="store_true",
        help="claim the next pending batch and print it as JSON",
    )
    output.add_argument(
        "--retry",
        action="store_true",
        help="claim the next failed batch for retry and print it as JSON",
    )
    output.add_argument(
        "--failed",
        action="store_true",
        help="print failed batches as JSON",
    )
    output.add_argument(
        "--status",
        action="store_true",
        help="print a compact JSON progress summary",
    )

    action = parser.add_mutually_exclusive_group()
    action.add_argument(
        "--mark-success",
        type=int,
        metavar="N",
        help="mark claimed batch N as successfully analyzed",
    )
    action.add_argument(
        "--mark-failed",
        type=int,
        metavar="N",
        help="mark claimed batch N as failed",
    )
    action.add_argument(
        "--reset-state",
        action="store_true",
        help="reset all batches for the current screenshot set to pending",
    )
    action.add_argument(
        "--reclaim-in-progress",
        action="store_true",
        help="return interrupted in-progress batches to pending or failed",
    )
    parser.add_argument(
        "--error",
        default="",
        help="failure detail used with --mark-failed",
    )
    return parser.parse_args(argv)


def _mark_result(
    state_path: Path,
    directory: Path,
    images: Sequence[Path],
    batches: Sequence[Sequence[Path]],
    fingerprint: str,
    index: int,
    success: bool,
    error: str,
) -> None:
    state = _read_state(state_path)
    if state is None:
        raise StateError("no batch state exists; run --manifest or --claim first")
    if state.get("fingerprint") != fingerprint:
        raise StateError(
            "batch state is stale because the screenshot set changed; "
            "run --manifest before marking a result"
        )
    batch = _find_batch(state, index)
    if batch.get("status") != STATUS_IN_PROGRESS:
        raise StateError(
            f"batch {index} is {batch.get('status')}, not in_progress; claim it first"
        )
    batch["status"] = STATUS_SUCCESS if success else STATUS_FAILED
    batch["error"] = None if success else (error or "unspecified failure")
    _write_state(state_path, state)


def _reset_state(
    state_path: Path,
    directory: Path,
    images: Sequence[Path],
    batches: Sequence[Sequence[Path]],
    fingerprint: str,
) -> dict[str, object]:
    state = _new_state(directory, images, batches, fingerprint)
    _write_state(state_path, state)
    return state


def _reclaim_in_progress(state_path: Path) -> int:
    state = _read_state(state_path)
    if state is None:
        raise StateError("no batch state exists; run --manifest or --claim first")
    reclaimed = 0
    for batch in _state_batches(state):
        if batch.get("status") != STATUS_IN_PROGRESS:
            continue
        # A prior failure remains meaningful after an interrupted retry.
        batch["status"] = STATUS_FAILED if batch.get("error") else STATUS_PENDING
        reclaimed += 1
    if reclaimed:
        _write_state(state_path, state)
    return reclaimed


def main(argv: Sequence[str] | None = None) -> int:
    args = _parse_args(argv)

    try:
        directory = args.dir.expanduser()
        if not directory.is_absolute():
            directory = (PROJECT_ROOT / directory).resolve()
        state_path = _resolve_state_path(args.state)
        _validate_state_location(state_path, directory)
        images, batches, fingerprint = _current_plan(directory)
    except (OSError, RuntimeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1

    if not images:
        print(f"error: no supported images found in {_display_path(directory)}", file=sys.stderr)
        return 1

    try:
        if args.manifest:
            with _state_lock(state_path):
                state = _ensure_state(
                    state_path, directory, images, batches, fingerprint
                )
                print(_manifest(directory, images, batches, state))
            return 0

        if args.batch is not None:
            if args.batch < 1 or args.batch > len(batches):
                print(
                    f"error: batch must be between 1 and {len(batches)}",
                    file=sys.stderr,
                )
                return 1
            for path in batches[args.batch - 1]:
                print(_display_path(path))
            return 0

        if args.claim:
            with _state_lock(state_path):
                state = _ensure_state(
                    state_path, directory, images, batches, fingerprint
                )
                claimed = _claim_batch(state, STATUS_PENDING)
                _write_state(state_path, state)
                print(_claim_output(claimed))
            return 0

        if args.retry:
            with _state_lock(state_path):
                state = _read_state(state_path)
                if state is None:
                    raise StateError("no batch state exists; run --manifest first")
                if state.get("fingerprint") != fingerprint:
                    raise StateError(
                        "batch state is stale because the screenshot set changed; "
                        "run --manifest before retrying"
                    )
                claimed = _claim_batch(state, STATUS_FAILED)
                _write_state(state_path, state)
                print(_claim_output(claimed))
            return 0

        if args.failed:
            with _state_lock(state_path):
                state = _ensure_state(
                    state_path, directory, images, batches, fingerprint
                )
                failed = [
                    batch
                    for batch in _state_batches(state)
                    if batch.get("status") == STATUS_FAILED
                ]
                print(json.dumps(failed, indent=2, ensure_ascii=False))
            return 0

        if args.status:
            with _state_lock(state_path):
                state = _ensure_state(
                    state_path, directory, images, batches, fingerprint
                )
                print(_state_summary(state))
            return 0

        if args.mark_success is not None or args.mark_failed is not None:
            index = (
                args.mark_success
                if args.mark_success is not None
                else args.mark_failed
            )
            assert index is not None
            with _state_lock(state_path):
                _mark_result(
                    state_path,
                    directory,
                    images,
                    batches,
                    fingerprint,
                    index,
                    success=args.mark_success is not None,
                    error=args.error,
                )
            print(f"batch {index}: {'success' if args.mark_success is not None else 'failed'}")
            return 0

        if args.reset_state:
            with _state_lock(state_path):
                state = _reset_state(
                    state_path, directory, images, batches, fingerprint
                )
                print(_state_summary(state))
            return 0

        if args.reclaim_in_progress:
            with _state_lock(state_path):
                count = _reclaim_in_progress(state_path)
            print(f"reclaimed {count} interrupted batch(es)")
            return 0

        _print_text(directory, images, batches)
        return 0
    except (OSError, RuntimeError, ValueError) as error:
        print(f"error: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
