use inc::Module::Install;
use Devel::CheckLib;
name 'Devel-BindPP';
all_from 'lib/Devel/BindPP.pm';

perl_version '5.008005';
license 'perl';
check_lib_or_exit(lib => 'stdc++');

requires 'File::ShareDir';
requires 'File::Copy';

my $ccflags = '';
if ($ENV{USER} eq 'tokuhirom') {
    $ccflags = '-g -Wall -O2 -Wformat=2 -Wcast-qual -Wcast-align -Wwrite-strings -Wconversion -Wfloat-equal -Wpointer-arith -fstack-protector-all';
}

makemaker_args(
    OBJECT => '$(O_FILES)',
    LIBS => ['-lstdc++'],
    CCFLAGS => $ccflags,
    CC => 'g++',
    LD => 'g++',
    XSPOT => '-C++',
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

can_cc or die "This module requires a C++ compiler";

tests 't/*.t';
author_tests 'xt';

auto_set_repository;
install_share 'share';
build_requires 'Test::More';
build_requires 'Devel::CheckLib';
use_test_base;
auto_include;
WriteAll;

