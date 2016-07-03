{
    "target_defaults": {
        "default_configuration": "Release",
        "sources": [
            "<!@(ls -1 src/*.cc)",
            "<!@(ls -1 src/AdBlockPlusFilter/*.cc)",
            "<!@(ls -1 src/utils/*.cc)"
        ],
        "include_dirs": [
            "ceflib"
        ],
        "library_dirs": [
            "ceflib"
        ],
        "ldflags": [
            "-Wl,-R,."
        ],
        "libraries": [
            "-lboost_regex",
            "-lboost_system",
            "-lboost_filesystem",
            "-lproxygenlib",
            "-lcrypto",
            "-lglog",
            "-ldouble-conversion",
            "-lpng",
            "-lpthread",
            "-lgflags",
            "-lproxygenhttpserver",
            "-lfolly",
            "-lcef",
            "-lcef_dll_wrapper"
        ],
        "cflags": [
            "-Wall",
            "-c",
            "-fmessage-length=0",
            "-std=c++11",
            "-MMD",
            "-MP"
        ],
        "configurations": {
            "Release": {
                "cflags": [
                    "-O2"
                ]
            },
            "Debug": {
                "cflags": [
                    "-g3"
                ]
            }
        }
    },
    "targets": [
        {
            "target_name": "Roxxy",
            "type": "executable",
            "defines": [
                "ROXXY_BUILD"
            ]
        },
        {
            "target_name": "Test",
            "type": "executable",
            "sources": [
                "<!@(ls -1 src/roxxyTest/cpp/*.cc)"
            ],
            "libraries": [
                "-lgtest",
                "-lgmock"
            ]
        },
        {
            "target_name": "copy_roxxy_dependencies",
            "type": "none",
            "dependencies": [
                "Roxxy",
                "Test"
            ],
            "copies": [
                {
                    "destination": "<(PRODUCT_DIR)",
                    "files": [
                        "<!@(ls -1 ceflib/*.pak)",
                        "<!@(ls -1 ceflib/*.dat)",
                        "<!@(ls -1 ceflib/*.so)",
                        "<!@(ls -1 ceflib/*.bin)",
                        "assets/filters",
                        "ceflib/locales"
                    ]
                }
            ]
        }
    ]
}