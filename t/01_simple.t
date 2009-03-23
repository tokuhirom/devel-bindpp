use strict;
use warnings;
use Test::More tests => 7;

use Devel::BindPP;

is Devel::BindPP::twice(5), 10;
is Devel::BindPP::catfoo('hoge'), 'hogefoo';
is Devel::BindPP::twice_n(3.2), 6.4;
is Devel::BindPP::hvref_fetch(+{a => 'b'}, 'a'), 'b';
is Devel::BindPP::hvref_fetch(+{a => 'b'}, 'c'), undef;
is Devel::BindPP::twice_u(5), 10;

# array
is Devel::BindPP::avref_fetch([qw/a b c/], 1), 'b';

