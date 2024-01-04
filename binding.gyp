{
  'targets': [
    {
      'target_name': 'hello',
      "dependencies": [
        'libwebrtc.gyp:libwebrtc'
      ],
      'sources': [
        "hello_world.cc"
      ],
      'conditions':
      [
        [ 'OS != "win"', {
          'cflags': [ '-std=c++11', '-Wall', '-Wextra', '-Wno-unused-parameter','-fexceptions' ]
        }],
        [ 'OS == "mac"', {
          'xcode_settings':
          {
            'OTHER_CPLUSPLUSFLAGS' : [ '-std=c++11' ],
            'WARNING_CFLAGS': [ '-Wall', '-Wextra', '-Wno-unused-parameter','-fexceptions' ],
          }
        }]
      ]
    }
  ]
}