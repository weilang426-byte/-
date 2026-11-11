import html
import os
import re
import sys
from xml.sax.saxutils import escape

from reportlab.lib import colors
from reportlab.lib.enums import TA_CENTER, TA_LEFT
from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import ParagraphStyle, getSampleStyleSheet
from reportlab.lib.units import mm
from reportlab.pdfbase import pdfmetrics
from reportlab.pdfbase.ttfonts import TTFont
from reportlab.platypus import (
    HRFlowable,
    KeepTogether,
    PageBreak,
    Paragraph,
    Preformatted,
    SimpleDocTemplate,
    Spacer,
    Table,
    TableStyle,
)


def inline_markup(text):
    text = escape(text)
    text = re.sub(r"`([^`]+)`", r"<font name='CJKMono'>\1</font>", text)
    text = re.sub(r"\*\*([^*]+)\*\*", r"<b>\1</b>", text)
    text = re.sub(r"\*([^*]+)\*", r"<i>\1</i>", text)
    return text


def parse_table(lines):
    rows = []
    for line in lines:
        if re.match(r"^\s*\|?\s*:?-+:?\s*(\|\s*:?-+:?\s*)+\|?\s*$", line):
            continue
        cells = [cell.strip() for cell in line.strip().strip("|").split("|")]
        rows.append(cells)
    return rows


def build_story(md_text, styles):
    story = []
    lines = md_text.replace("\r\n", "\n").split("\n")
    i = 0
    paragraph_buf = []

    def flush_paragraph():
        nonlocal paragraph_buf
        if paragraph_buf:
            text = " ".join(x.strip() for x in paragraph_buf if x.strip())
            if text:
                story.append(Paragraph(inline_markup(text), styles["Body"]))
                story.append(Spacer(1, 2.2 * mm))
            paragraph_buf = []

    while i < len(lines):
        line = lines[i]

        if line.startswith("```"):
            flush_paragraph()
            code_lines = []
            i += 1
            while i < len(lines) and not lines[i].startswith("```"):
                code_lines.append(lines[i])
                i += 1
            story.append(Preformatted("\n".join(code_lines), styles["Code"]))
            story.append(Spacer(1, 2.5 * mm))
            i += 1
            continue

        if not line.strip():
            flush_paragraph()
            i += 1
            continue

        heading = re.match(r"^(#{1,4})\s+(.*)$", line)
        if heading:
            flush_paragraph()
            level = len(heading.group(1))
            style = styles[f"H{min(level, 4)}"]
            story.append(Paragraph(inline_markup(heading.group(2).strip()), style))
            i += 1
            continue

        if re.match(r"^\s*---+\s*$", line):
            flush_paragraph()
            story.append(Spacer(1, 1.5 * mm))
            story.append(HRFlowable(width="100%", thickness=0.6, color=colors.HexColor("#AAB4C0")))
            story.append(Spacer(1, 2.5 * mm))
            i += 1
            continue

        if line.startswith("|") and i + 1 < len(lines) and "|" in lines[i + 1]:
            flush_paragraph()
            table_lines = [line]
            i += 1
            while i < len(lines) and lines[i].strip().startswith("|"):
                table_lines.append(lines[i])
                i += 1
            rows = parse_table(table_lines)
            if rows:
                data = []
                for row_index, row in enumerate(rows):
                    cells = []
                    for cell in row:
                        cell_style = styles["TableHeader"] if row_index == 0 else styles["TableCell"]
                        cells.append(Paragraph(inline_markup(cell), cell_style))
                    data.append(cells)
                col_count = max(len(r) for r in data)
                for row in data:
                    row.extend([""] * (col_count - len(row)))
                usable_width = 170 * mm
                widths = [usable_width / col_count] * col_count
                table = Table(data, colWidths=widths, repeatRows=1, hAlign="LEFT")
                table.setStyle(TableStyle([
                    ("BACKGROUND", (0, 0), (-1, 0), colors.HexColor("#DCE6F1")),
                    ("TEXTCOLOR", (0, 0), (-1, 0), colors.HexColor("#1F2933")),
                    ("GRID", (0, 0), (-1, -1), 0.35, colors.HexColor("#AAB4C0")),
                    ("VALIGN", (0, 0), (-1, -1), "TOP"),
                    ("LEFTPADDING", (0, 0), (-1, -1), 4),
                    ("RIGHTPADDING", (0, 0), (-1, -1), 4),
                    ("TOPPADDING", (0, 0), (-1, -1), 4),
                    ("BOTTOMPADDING", (0, 0), (-1, -1), 4),
                ]))
                story.append(table)
                story.append(Spacer(1, 3 * mm))
            continue

        bullet = re.match(r"^\s*([-*+])\s+(.*)$", line)
        numbered = re.match(r"^\s*(\d+)\.\s+(.*)$", line)
        if bullet or numbered:
            flush_paragraph()
            if bullet:
                label, text = "•", bullet.group(2)
            else:
                label, text = numbered.group(1) + ".", numbered.group(2)
            story.append(Paragraph(f"<b>{label}</b>&nbsp;&nbsp;{inline_markup(text)}", styles["Bullet"]))
            i += 1
            continue

        paragraph_buf.append(line)
        i += 1

    flush_paragraph()
    return story


def add_page_number(canvas, doc):
    canvas.saveState()
    canvas.setFont("CJK", 8)
    canvas.setFillColor(colors.HexColor("#667085"))
    canvas.drawString(20 * mm, 12 * mm, "FreeRTOS、CMSIS-RTOS2 与 STM32 HAL")
    canvas.drawRightString(190 * mm, 12 * mm, f"{doc.page}")
    canvas.restoreState()


def main():
    if len(sys.argv) != 3:
        raise SystemExit("usage: md_to_pdf.py input.md output.pdf")

    source, output = sys.argv[1], sys.argv[2]
    os.makedirs(os.path.dirname(output), exist_ok=True)

    pdfmetrics.registerFont(TTFont("CJK", r"C:\Windows\Fonts\NotoSansSC-VF.ttf"))
    pdfmetrics.registerFont(TTFont("CJKMono", r"C:\Windows\Fonts\Deng.ttf"))

    base = getSampleStyleSheet()
    styles = {
        "Body": ParagraphStyle(
            "Body", parent=base["BodyText"], fontName="CJK", fontSize=9.3,
            leading=14, spaceAfter=1.5 * mm, alignment=TA_LEFT,
        ),
        "Bullet": ParagraphStyle(
            "Bullet", parent=base["BodyText"], fontName="CJK", fontSize=9.2,
            leading=13.5, leftIndent=5 * mm, firstLineIndent=-5 * mm,
            spaceAfter=1.2 * mm,
        ),
        "H1": ParagraphStyle(
            "H1", parent=base["Heading1"], fontName="CJK", fontSize=18,
            leading=23, textColor=colors.HexColor("#17324D"), spaceBefore=3 * mm,
            spaceAfter=4 * mm,
        ),
        "H2": ParagraphStyle(
            "H2", parent=base["Heading2"], fontName="CJK", fontSize=14,
            leading=18, textColor=colors.HexColor("#245B7A"), spaceBefore=3 * mm,
            spaceAfter=2.5 * mm,
        ),
        "H3": ParagraphStyle(
            "H3", parent=base["Heading3"], fontName="CJK", fontSize=11.5,
            leading=15, textColor=colors.HexColor("#2F4858"), spaceBefore=2.5 * mm,
            spaceAfter=1.5 * mm,
        ),
        "H4": ParagraphStyle(
            "H4", parent=base["Heading4"], fontName="CJK", fontSize=10.2,
            leading=13, textColor=colors.HexColor("#435A6B"), spaceBefore=2 * mm,
            spaceAfter=1 * mm,
        ),
        "Code": ParagraphStyle(
            "Code", fontName="CJKMono", fontSize=7.1, leading=9,
            leftIndent=4 * mm, rightIndent=4 * mm,
            backColor=colors.HexColor("#F2F4F7"),
            borderColor=colors.HexColor("#D0D5DD"), borderWidth=0.35,
            borderPadding=4, spaceBefore=1 * mm, spaceAfter=2 * mm,
        ),
        "TableHeader": ParagraphStyle(
            "TableHeader", fontName="CJK", fontSize=7.8, leading=10,
            textColor=colors.HexColor("#1F2933"),
        ),
        "TableCell": ParagraphStyle(
            "TableCell", fontName="CJK", fontSize=7.6, leading=9.5,
        ),
    }

    with open(source, "r", encoding="utf-8") as f:
        text = f.read()

    doc = SimpleDocTemplate(
        output,
        pagesize=A4,
        rightMargin=20 * mm,
        leftMargin=20 * mm,
        topMargin=17 * mm,
        bottomMargin=18 * mm,
        title="FreeRTOS、CMSIS-RTOS2 与 STM32 HAL 基础与工程实践",
        author="Codex",
    )
    story = build_story(text, styles)
    doc.build(story, onFirstPage=add_page_number, onLaterPages=add_page_number)
    print(output)


if __name__ == "__main__":
    main()
