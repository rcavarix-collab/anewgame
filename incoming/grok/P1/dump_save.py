#!/usr/bin/env python3
"""Read-only dump of a walkgrid save file (little-endian binary format)."""

import sys
import struct
from collections import Counter

FNV_OFFSET = 2166136261
FNV_PRIME = 16777619
CHUNK_CELLS = 4096


class TruncatedError(Exception):
    pass


class BadMagicError(Exception):
    pass


class ParseError(Exception):
    pass


def _need(data: bytes, offset: int, n: int) -> None:
    if offset + n > len(data):
        raise TruncatedError(
            f"need {n} byte(s) at offset {offset}, only {len(data) - offset} left"
        )


def read_u8(data: bytes, offset: int) -> tuple[int, int]:
    _need(data, offset, 1)
    return data[offset], offset + 1


def read_u16(data: bytes, offset: int) -> tuple[int, int]:
    _need(data, offset, 2)
    return struct.unpack_from("<H", data, offset)[0], offset + 2


def read_i32(data: bytes, offset: int) -> tuple[int, int]:
    _need(data, offset, 4)
    return struct.unpack_from("<i", data, offset)[0], offset + 4


def read_u32(data: bytes, offset: int) -> tuple[int, int]:
    _need(data, offset, 4)
    return struct.unpack_from("<I", data, offset)[0], offset + 4


def read_u64(data: bytes, offset: int) -> tuple[int, int]:
    _need(data, offset, 8)
    return struct.unpack_from("<Q", data, offset)[0], offset + 8


def read_f32(data: bytes, offset: int) -> tuple[float, int]:
    _need(data, offset, 4)
    return struct.unpack_from("<f", data, offset)[0], offset + 4


def read_str(data: bytes, offset: int) -> tuple[str, int]:
    length, offset = read_u16(data, offset)
    _need(data, offset, length)
    try:
        s = data[offset : offset + length].decode("utf-8")
    except UnicodeDecodeError as e:
        raise ParseError(f"invalid UTF-8 in string at offset {offset}: {e}") from e
    return s, offset + length


def read_bytes(data: bytes, offset: int, n: int) -> tuple[bytes, int]:
    _need(data, offset, n)
    return data[offset : offset + n], offset + n


def fnv1a_32(data: bytes) -> int:
    h = FNV_OFFSET
    for b in data:
        h ^= b
        h = (h * FNV_PRIME) & 0xFFFFFFFF
    return h


def parse_runs_u16_u16(data: bytes, offset: int, expected: int) -> tuple[list[tuple[int, int]], int]:
    """Runs of (u16 length, u16 value) that must cover exactly `expected` cells."""
    runs = []
    covered = 0
    while covered < expected:
        length, offset = read_u16(data, offset)
        value, offset = read_u16(data, offset)
        if length == 0:
            raise ParseError(f"zero-length run at offset {offset - 4}")
        if covered + length > expected:
            raise ParseError(
                f"run length {length} would exceed {expected} cells (covered {covered})"
            )
        runs.append((length, value))
        covered += length
    if covered != expected:
        raise ParseError(f"runs covered {covered} cells, expected {expected}")
    return runs, offset


def parse_runs_u16_u8(data: bytes, offset: int, expected: int) -> tuple[list[tuple[int, int]], int]:
    """Runs of (u16 length, u8 value) that must cover exactly `expected` cells."""
    runs = []
    covered = 0
    while covered < expected:
        length, offset = read_u16(data, offset)
        value, offset = read_u8(data, offset)
        if length == 0:
            raise ParseError(f"zero-length state run at offset {offset - 3}")
        if covered + length > expected:
            raise ParseError(
                f"state run length {length} would exceed {expected} cells (covered {covered})"
            )
        runs.append((length, value))
        covered += length
    if covered != expected:
        raise ParseError(f"state runs covered {covered} cells, expected {expected}")
    return runs, offset


def dump_save(path: str) -> None:
    try:
        with open(path, "rb") as f:
            data = f.read()
    except OSError as e:
        print(f"error: cannot read {path}: {e}")
        return

    if len(data) < 8:
        print("error: file too short for magic and version")
        return

    offset = 0
    try:
        magic, offset = read_u32(data, offset)
        if magic != 0x44524757:  # 'WGRD' little-endian
            # also accept the four bytes in file order
            if data[0:4] != b"WGRD":
                raise BadMagicError(
                    f"bad magic {magic:#010x} (expected WGRD / 0x44524757)"
                )
        version, offset = read_u32(data, offset)

        # player
        px, offset = read_f32(data, offset)
        py, offset = read_f32(data, offset)
        pz, offset = read_f32(data, offset)
        yaw, offset = read_f32(data, offset)
        pitch, offset = read_f32(data, offset)
        hotbar, offset = read_i32(data, offset)
        time_of_day, offset = read_f32(data, offset)

        # generator
        gen_name, offset = read_str(data, offset)
        gen_version, offset = read_u32(data, offset)
        gen_seed, offset = read_u64(data, offset)

        # block names
        name_count, offset = read_u32(data, offset)
        block_names: list[str] = []
        for i in range(name_count):
            name, offset = read_str(data, offset)
            block_names.append(name)

        # chunks
        chunk_count, offset = read_u32(data, offset)
        cell_counts: Counter[str] = Counter()
        for _ in range(chunk_count):
            cx, offset = read_i32(data, offset)
            cy, offset = read_i32(data, offset)
            cz, offset = read_i32(data, offset)
            flags, offset = read_u8(data, offset)

            # blocks: runs covering 4096 cells
            block_runs, offset = parse_runs_u16_u16(data, offset, CHUNK_CELLS)
            for length, name_idx in block_runs:
                if name_idx >= len(block_names):
                    raise ParseError(
                        f"block name index {name_idx} out of range "
                        f"(only {len(block_names)} names)"
                    )
                cell_counts[block_names[name_idx]] += length

            # optional state
            if flags & 1:
                _, offset = parse_runs_u16_u8(data, offset, CHUNK_CELLS)

            # optional data
            if flags & 2:
                data_count, offset = read_u16(data, offset)
                for _ in range(data_count):
                    cell, offset = read_u16(data, offset)
                    if cell >= CHUNK_CELLS:
                        raise ParseError(f"data cell index {cell} >= {CHUNK_CELLS}")
                    dlen, offset = read_u32(data, offset)
                    _, offset = read_bytes(data, offset, dlen)

        # updates
        update_count, offset = read_u32(data, offset)
        for _ in range(update_count):
            _, offset = read_i32(data, offset)  # x
            _, offset = read_i32(data, offset)  # y
            _, offset = read_i32(data, offset)  # z
            _, offset = read_u8(data, offset)   # kind
            _, offset = read_u32(data, offset)  # delay

        # game section
        game_len, offset = read_u32(data, offset)
        game_bytes, offset = read_bytes(data, offset, game_len)

        day_count = None
        if game_len >= 4 and game_bytes[0:4] == b"DAY1":
            if game_len >= 8:
                day_count = struct.unpack_from("<I", game_bytes, 4)[0]
            # else: starts with DAY1 but too short for the u32 → leave as None

        # checksum (last 4 bytes)
        if offset + 4 > len(data):
            raise TruncatedError("missing checksum")
        stored_checksum, offset = read_u32(data, offset)
        if offset != len(data):
            # trailing junk is allowed to be reported but does not fail the parse
            pass

        computed = fnv1a_32(data[: offset - 4])
        checksum_ok = computed == stored_checksum

        # ---- output ----
        print(f"version: {version}")
        print(f"player: pos=({px:.6g}, {py:.6g}, {pz:.6g}) "
              f"yaw={yaw:.6g} pitch={pitch:.6g} hotbar={hotbar}")
        print(f"time of day: {time_of_day:.6g} s")
        if day_count is not None:
            print(f"day count: {day_count}")
        else:
            print("day count: (not present)")
        print(f"generator: name={gen_name!r} version={gen_version} seed={gen_seed}")
        print(f"block names ({len(block_names)}):")
        for i, n in enumerate(block_names):
            print(f"  [{i}] {n}")
        print(f"chunk count: {chunk_count}")
        print("cells per block name (across stored chunks):")
        if not cell_counts:
            print("  (none)")
        else:
            for name in block_names:
                if name in cell_counts:
                    print(f"  {name}: {cell_counts[name]}")
            # any unexpected names (should not happen)
            for name, cnt in cell_counts.items():
                if name not in block_names:
                    print(f"  {name}: {cnt}")
        print(f"checksum: {'OK' if checksum_ok else 'MISMATCH'} "
              f"(stored={stored_checksum:#010x}, computed={computed:#010x})")

    except BadMagicError as e:
        print(f"error: {e}")
    except TruncatedError as e:
        print(f"error: truncated or damaged file: {e}")
    except ParseError as e:
        print(f"error: parse error: {e}")
    except struct.error as e:
        print(f"error: binary unpack failed: {e}")
    except Exception as e:
        print(f"error: unexpected: {type(e).__name__}: {e}")


def main() -> None:
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <savefile.sav>", file=sys.stderr)
        sys.exit(1)
    dump_save(sys.argv[1])


if __name__ == "__main__":
    main()
