const STARTX = $nu.home-path | path join "documents/github.com/lslvr/startx/startx"

const SRC  = "llwm.c"
const EXEC = "./build/llwm"

const CC = "gcc"
const CFLAGS = [ "-Wno-trigraphs" ]
const LDFLAGS = [ "-lX11" ]

def --wrapped cmd-run [cmd, ...args] {
    print $"($cmd) ($args | str join ' ')"
    run-external $cmd ...$args
}

export def build [
    src: string = $SRC,
    --output (-o): string = $EXEC,
    --cc: string = $CC,
    --cflags: list<string> = $CFLAGS,
    --ldflags: list<string> = $LDFLAGS,
] {
    if not ($EXEC | path dirname | path exists) {
        print $"mkdir ($EXEC | path dirname)"
        mkdir ($EXEC | path dirname)
    }
    cmd-run $cc ...$cflags -o $output $src ...$ldflags -DDEBUG
}

export def run [] { cmd-run $STARTX $EXEC }

alias "core kill" = kill
export def kill  [] {
    for proc in (ps | find Xorg) {
        print $"kill ($proc.pid)"
        core kill $proc.pid
    }
}
