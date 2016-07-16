{
    "target_defaults": {
        "default_configuration": "Release",
        "sources": [
            "<!@(ls -1 src/*.cc)",
            "<!@(ls -1 src/AdBlockPlusFilter/*.cc)",
            "<!@(ls -1 src/utils/*.cc)"
        ],
        "include_dirs": [
            "cef_binary_3.2743.1442.ge29124d_linux64"
        ],
        "library_dirs": [
            "cef_binary_3.2743.1442.ge29124d_linux64/Release",
            "cef_binary_3.2743.1442.ge29124d_linux64/libcef_dll_wrapper"
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
                        "cef_binary_3.2743.1442.ge29124d_linux64/Release/chrome-sandbox",
			"<!@(ls -1 cef_binary_3.2743.1442.ge29124d_linux64/Resources/*.pak)",
                        "<!@(ls -1 cef_binary_3.2743.1442.ge29124d_linux64/Resources/*.dat)",
                        "<!@(ls -1 cef_binary_3.2743.1442.ge29124d_linux64/Release/*.so)",
                        "<!@(ls -1 cef_binary_3.2743.1442.ge29124d_linux64/Release/*.bin)",
                        "assets/filters",
                        "cef_binary_3.2743.1442.ge29124d_linux64/Resources/locales"
                    ]
                }
            ]
        }
    ]
}
