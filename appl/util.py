import re

class KConfigFile(dict):
    """A collection of KConfig settings. Stores them as key, value pairs."""
    def __init__(self, conf: str):
        dict.__init__(self)

        with open(conf, 'r') as f:
            # Regex used to delete all whitespace, comments and quotes from the key=value entry.
            strip = re.compile(r'\s+|"|#.*')
            for line in f.readlines():
                line = re.sub(strip, '', line)
                tokens = line.split('=')

                if len(tokens) != 2:
                    continue

                self[tokens[0]] = tokens[1]
