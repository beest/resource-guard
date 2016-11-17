{
    "targets": [
        {
            "target_name": "guard",
            "sources": [
                "src/guard.cc"
            ],
            "cflags": [
                "-std=c++11"
            ],
            "conditions": [
                [
                    'OS=="mac"', {
                        'xcode_settings': {
                            'OTHER_CPLUSPLUSFLAGS' : ['-std=c++11','-stdlib=libc++'],
                            'OTHER_LDFLAGS': ['-stdlib=libc++'],
                        },
                    }
                ],
            ]
        }
    ]
}
