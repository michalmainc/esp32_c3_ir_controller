from pathlib import Path

Import("env")

PROJECT_DIR = Path(env["PROJECT_DIR"])

ASSETS = [
    ("web_index_html", PROJECT_DIR / "src/web/assets/index.html"),
    ("web_style_css", PROJECT_DIR / "src/web/assets/style.css"),
    ("web_app_js", PROJECT_DIR / "src/web/assets/app.js"),
]

OUTPUT_C = PROJECT_DIR / "src/web/generated_assets.c"
OUTPUT_H = PROJECT_DIR / "src/web/generated_assets.h"


def format_byte_array(data: bytes) -> str:
    values = [f"0x{value:02X}" for value in data + b"\0"]

    lines = []

    for index in range(0, len(values), 12):
        lines.append("    " + ", ".join(values[index:index + 12]))

    return ",\n".join(lines)


def write_if_changed(path: Path, content: str) -> None:
    if path.exists() and path.read_text(encoding="utf-8") == content:
        return

    path.write_text(content, encoding="utf-8")


header_lines = [
    "#ifndef GENERATED_WEB_ASSETS_H",
    "#define GENERATED_WEB_ASSETS_H",
    "",
    "#include <stddef.h>",
    "",
]

source_lines = [
    '#include "generated_assets.h"',
    "",
]

for symbol, asset_path in ASSETS:
    if not asset_path.exists():
        raise FileNotFoundError(
            f"Nie znaleziono pliku zasobu: {asset_path}"
        )

    data = asset_path.read_bytes()

    header_lines.extend([
        f"extern const unsigned char {symbol}[];",
        f"extern const size_t {symbol}_length;",
        "",
    ])

    source_lines.extend([
        f"const unsigned char {symbol}[] = {{",
        format_byte_array(data),
        "};",
        "",
        f"const size_t {symbol}_length =",
        f"    sizeof({symbol}) - 1;",
        "",
    ])

header_lines.extend([
    "#endif",
    "",
])

write_if_changed(
    OUTPUT_H,
    "\n".join(header_lines)
)

write_if_changed(
    OUTPUT_C,
    "\n".join(source_lines)
)

print("Web assets generated.")