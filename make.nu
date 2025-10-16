const BUILD_DIR = "./build/"

const CC = "gcc"
const CFLAGS = [ "-Wno-trigraphs" ]
const LDFLAGS = [ "-lX11" ]
const DDEFS = [ "-DDEBUG" ]

def --wrapped cmd-run [cmd, ...args] {
    print $"($cmd) ($args | str join ' ')"
    run-external $cmd ...$args
}

export def compile [
    ...src: string,
    --cc: string = $CC,
    --cflags: list<string> = $CFLAGS,
    --ldflags: list<string> = $LDFLAGS,
    --ddefs: list<string> = $DDEFS,
] {
    if not ($BUILD_DIR | path exists) {
        print $"mkdir ($BUILD_DIR)"
        mkdir $BUILD_DIR
    }

    for s in $src {
        let output = $s | path parse | update parent "build" | update extension "o" | path join
        cmd-run $cc ...$cflags -c -o $output $s ...$ldflags ...$ddefs
    }
}

export def link [
    ...objs: string,
    --output: string = "a.out",
    --cc: string = $CC,
    --cflags: list<string> = $CFLAGS,
    --ldflags: list<string> = $LDFLAGS,
    --ddefs: list<string> = $DDEFS,
] {
    cmd-run $cc ...$cflags -o $output ...$objs ...$ldflags ...$ddefs
}

alias "core kill" = kill
export def kill  [] {
    for proc in (ps | find Xorg) {
        print $"kill ($proc.pid)"
        core kill $proc.pid
    }
}
