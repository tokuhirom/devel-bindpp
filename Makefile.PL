use inc::Module::Install;
use Devel::CheckLib;
name 'Devel-BindPP';
all_from 'lib/Devel/BindPP.pm';

check_lib_or_exit(lib => 'stdc++');
can_cc or die "This module requires a C++ compiler";

my $ccflags = '';
if ($ENV{USER} eq 'tokuhirom') {
    $ccflags = '-g -Wall -O2 -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith -fstack-protector-all';
}

makemaker_args(
    OBJECT => '$(O_FILES)',
    LIBS => ['-lstdc++'],
    CCFLAGS => $ccflags,
    clean => {
        FILES => q{
            *.stackdump
            *.gcov *.gcda *.gcno
            *.out
            nytprof
            cover_db
        },
    },
);

tests 't/*.t';
author_tests 'xt';

build_requires 'Test::More';
use_test_base;
auto_include;
WriteAll;

