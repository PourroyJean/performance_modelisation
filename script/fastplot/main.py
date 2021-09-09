from argparse import ArgumentParser, HelpFormatter
from os import path
import itertools
import sys
import os

import plotly.graph_objects as go


PREDEFINED_FILE_EXT = {
    ".csv": ([",*"], False),  # Don't skip empty values in CSV
    ".tsv": (["\t*"], False), # Don't skip empty values in TSV
}
DEFAULT_FILE_EXT = ([" *"], True)  # Default file extensions uses ' ' separator and skip empty values.


def main():

    parser = build_arg_parser()
    args = parser.parse_args()

    file_path = args.input
    _, file_ext = path.splitext(file_path)
    file_ext_seps, file_ext_skip_empty = PREDEFINED_FILE_EXT.get(file_ext, DEFAULT_FILE_EXT)

    log(f"Input file: {file_path}")

    raw_separators = args.sep
    raw_skip_empty = args.skip_empty

    # Simple flags arguments
    header = args.header == "yes"
    strip = args.strip == "yes"

    # If separators are not set, use default one for common extensions.
    if raw_separators is None or not len(raw_separators):
        raw_separators = file_ext_seps

    log(f"Parsing separators...")

    # Pre process separators expressions
    separators = []
    for raw_sep in raw_separators:
        if raw_sep[-1] == "*":
            separators.append((raw_sep[:-1], -1))
        else:
            factor_idx = raw_sep.rfind("*")
            if factor_idx == -1:
                separators.append((raw_sep, 1))
            else:
                raw_factor = raw_sep[(factor_idx + 1):]
                try:
                    # Here, the given string IS not empty (check first if == "*").
                    factor = int(raw_factor)
                    separators.append((raw_sep[:factor_idx], factor))
                except ValueError:
                    print(f"Invalid factor '{raw_factor}' in '{raw_sep}'.")
                    sys.exit(1)

    if raw_skip_empty is None:
        # If skip empty is not set, set it to the file extension one.
        skip_empty = file_ext_skip_empty
    else:
        # If skip is set, just parse the bool value.
        skip_empty = raw_skip_empty == "yes"

    log(f"Parsing configuration:")
    log(f"  Stripping: {get_enabled_str(strip)}")
    log(f"  Skip empty: {get_enabled_str(skip_empty)}")
    log(f"  Separators: {', '.join(map(get_separator_str, separators))}")

    def append_filtered(line_: list, value: str):
        if strip:
            value = value.strip()
        if not skip_empty or len(value):
            line_.append(parse_float_or_str(value))

    lines = []

    with open(file_path, "rt") as fh:

        log(f"Parsing... 0%", end="\r")

        raw_lines = fh.readlines()
        raw_lines_count = len(raw_lines)
        separators_count = len(separators)

        for raw_line_idx, raw_line in enumerate(raw_lines):

            raw_line = raw_line.rstrip("\r\n")

            separator_idx = 0
            separator_count = 0
            search_idx = 0
            line = []

            while separator_idx < separators_count:

                sep, count = separators[separator_idx]
                sep_idx = raw_line.find(sep, search_idx)

                # Here, we check if the next separator, if existing, is placed before this separator.
                # This is used to avoid jumping over next expected separators.
                # For example in this line: ', ; : ,', with separators ',*' ';' we don't want to jump
                # from on comma to the last one.
                if sep_idx != -1:
                    next_separator_index = separator_idx + 1
                    while next_separator_index < separators_count:
                        next_sep, next_count = separators[separator_idx + 1]
                        next_sep_idx = raw_line.find(next_sep, search_idx)
                        if next_sep_idx == -1:
                            # If the next separator is not found but this separator has an undefined
                            # expected count, we just check the next separator.
                            if next_count == -1:
                                next_separator_index += 1
                                continue
                        elif next_sep_idx < sep_idx:
                            # If the separator is found and is before the current separator, set
                            # separator index to -1, it was not found.
                            sep_idx = -1
                        break

                if sep_idx == -1:

                    if count == -1:
                        separator_idx += 1
                    else:
                        # If the current separator is not found but was expected, ignore the line.
                        line = None
                        break

                else:

                    append_filtered(line, raw_line[search_idx:sep_idx])
                    search_idx = sep_idx + len(sep)

                    if count != -1:

                        # We only count separator if a count is expected.
                        separator_count += 1

                        if separator_count >= count:
                            # If the current
                            separator_idx += 1
                            separator_count = 0

            if line is not None:
                # We add the remaining part.
                append_filtered(line, raw_line[search_idx:])
                lines.append(line)

            log(f"Parsing... {int((raw_line_idx + 1) / raw_lines_count * 100)}%", end="\r")

    print()

    if not len(lines):
        log("No parsed line, terminating.")
        sys.exit(0)

    log("Successfully parsed!")

    array_axis = args.axis
    if array_axis is None:
        # Here we try to guess the array axis, by default it's in column mode.
        array_axis = "col"
        if isinstance(lines[0][0], str) and any(map(lambda value: not isinstance(value, str), lines[0][1:])):
            # Here, we know that line #0 has a string at first value, and subsequent values are not all strings.
            if all(map(lambda line_: isinstance(line_[0], str), lines[1:])):
                # Here we know that all lines after line #0 start with a string value.
                # In this case, we auto detect that this file's arrays are in rows.
                array_axis = "row"

    if array_axis == "row":
        arrays = lines
    else:
        # If the data axis is vertical, just transpose lines to columns.
        arrays = list(map(list, itertools.zip_longest(*lines)))

    if header:
        headers = []
        for array in arrays:
            headers.append(str(array.pop(0)).replace("_", " ").strip())
    else:
        headers = [f"#{i}" for i in range(len(arrays))]

    fig = go.Figure()

    if len(arrays) == 1:
        fig.add_trace(go.Scatter(x=map(lambda t: t[0], enumerate(arrays)), y=arrays[0], name=headers[0]))
        xtitle = ""
        ytitle = headers[0]
    else:
        for array, header in zip(arrays[1:], headers[1:]):
            fig.add_trace(go.Scatter(x=arrays[0], y=array, name=header))
        xtitle = headers[0]
        ytitle = get_common_end(headers[1:])

    if args.xtitle is not None:
        xtitle = args.xtitle
    if args.ytitle is not None:
        ytitle = args.ytitle

    fig.update_layout(
        title=f"FastPlot from {file_path}",
        xaxis_title=xtitle,
        yaxis_title=ytitle
    )

    if args.xlog:
        fig.update_xaxes(type="log")
    if args.ylog:
        fig.update_yaxes(type="log")

    log("Drawing summary:")
    log(f"  Headers: {', '.join(headers)}")
    log(f"  Arrays: {len(arrays)} ({array_axis}s)")
    log(f"  X title: {xtitle}")
    log(f"  Y title: {ytitle}")
    log(f"  X log scale: {get_enabled_str(args.xlog)}")
    log(f"  Y log scale: {get_enabled_str(args.ylog)}")

    output_file_path: str = args.output
    if output_file_path is not None:

        opts_idx = output_file_path.rfind("?")
        if opts_idx != -1:
            raw_opts = output_file_path[(opts_idx + 1):]
            output_file_path = output_file_path[:opts_idx]
        else:
            raw_opts = None

        _, output_file_ext = path.splitext(output_file_path)

        if output_file_ext in (".png", ".jpeg", ".jpg", ".svg", ".pdf"):

            log(f"Writing to image file at: {output_file_path}")

            size = [1280, 720]

            if raw_opts is None:
                log("No image size specified, using defaults (add '?<width>x<height>' after the path).")
            else:
                raw_opts_split = raw_opts.split("x", 1)
                raw_size = None
                if len(raw_opts_split) == 2:
                    try:
                        raw_size = [int(raw_opts_split[0]), int(raw_opts_split[1])]
                    except ValueError:
                        pass
                if raw_size is None:
                    log(f"Invalid image size '{raw_opts}', using defaults.")
                else:
                    size = raw_size

            log(f"Using image size {size[0]}x{size[1]}")

            os.makedirs(path.dirname(output_file_path), exist_ok=True)
            fig.write_image(output_file_path, width=size[0], height=size[1])

        elif output_file_ext in (".html", ".htm"):
            log(f"Writing to HTML file at: {output_file_path}")
            os.makedirs(path.dirname(output_file_path), exist_ok=True)
            fig.write_html(output_file_path)
        else:
            log(f"Unsupported output file format: {output_file_path}")
            sys.exit(1)

    else:
        log(f"Opening in your browser, if supported...")
        fig.show(renderer="browser")

    sys.exit(0)


def parse_float_or_str(raw):
    try:
        return float(raw)
    except ValueError:
        return raw


def get_common_end(strings):
    if not len(strings):
        return ""
    offset = -1
    while True:
        c = None
        for string in strings:
            try:
                if c is None:
                    c = string[offset]
                elif c != string[offset]:
                    c = None
                    break
            except IndexError:
                c = None
                break
        if c is None:
            break
        else:
            offset -= 1
    return "" if offset == -1 else strings[0][offset + 1:].strip()


def log(message, *, end="\n"):
    print(f"[FastPlot] {message}", end=end)


def get_enabled_str(enabled) -> str:
    return "enabled" if enabled else "disabled"


def get_separator_str(sep) -> str:
    if sep[1] == -1:
        return f"{sep[0]!r}*"
    else:
        return f"{sep[0]!r}*{sep[1]}"


def build_arg_parser() -> ArgumentParser:

    class LongerHelpFormatter(HelpFormatter):
        def __init__(self, *args, **kwargs):
            kwargs["max_help_position"] = 80
            kwargs["width"] = 100
            super().__init__(*args, **kwargs)

    parser = ArgumentParser(formatter_class=LongerHelpFormatter)

    # In this parser some argument use "yes"/"no" values instead of a flag argument, this is used to allow
    # computation of a default value depending on the input file extension and allowing the user to override
    # the parameter.

    parser.add_argument("input", help="Input table file.")

    parser.add_argument("-s", "--sep", help="Specify the separator used to split lines, by default this is guessed "
                                            "from the input file name. If you specify more than one separator they "
                                            "will be used in the given order to split a line.", nargs="*")

    parser.add_argument("--header", help="Enable or disable top header, this header is used for plot labels. "
                                         "If the header is disabled, default header '#<column_num>' are used.",
                        choices=["yes", "no"], default="yes")

    parser.add_argument("--strip", help="Enable or disable value stripping, by removing leading and trailing spaces. "
                                        "If you are using --skip-empty at the same time the stripping is done before "
                                        "actually testing length.",
                        choices=["yes", "no"], default="yes")

    parser.add_argument("--skip-empty", help="Enable or disable skipping of empty values on a line. "
                                             "The default value depends on the input file extension.",
                        choices=["yes", "no"])

    parser.add_argument("--axis", help="Specify the axis (column or row) of data arrays in the file. "
                                       "By default, column axis is used by the row axis is guessed by the program "
                                       "if all values in the first column are not decimals, and therefore can be"
                                       "used as row header.",
                        choices=["col", "row"])

    parser.add_argument("--xtitle", help="Specify the title for the X axis, by default it uses the header of the "
                                         "first column.")

    parser.add_argument("--ytitle", help="Specify the title for the Y axis, by default it uses the common end string "
                                         "of each columns after X column.")

    parser.add_argument("--xlog", help="Use log scale for X axis.", action="store_true")
    parser.add_argument("--ylog", help="Use log scale for Y axis.", action="store_true")

    parser.add_argument("-o", "--output", help="Specify an output file, by default the program will try to show "
                                               "the figure in your browser if available.")

    return parser


if __name__ == '__main__':
    main()
