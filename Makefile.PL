use inc::Module::Install;
name 'Devel-BindPP';
all_from 'lib/Devel/BindPP.pm';

can_cc or die "This module requires a C compiler";

makemaker_args(
    OBJECT => '$(O_FILES)',
    LIBS => ['-lstdc++'],
    CCFLAGS => '-Wall -O2',
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
