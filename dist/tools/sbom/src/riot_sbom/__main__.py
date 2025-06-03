"""
Copyright (C) 2025 ML!PA Consulting GmbH

SPDX-License-Identifier: MIT

Authors:
    Daniel Lockau <daniel.lockau@ml-pa.com>
"""

if __name__ == "__main__":
    import logging
    logging.basicConfig(level=logging.INFO)
    from .cli import main
    main()
