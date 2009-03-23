use strict;
use warnings;
use Test::More tests => 9;

use Devel::BindPP;

# scalar
is Devel::BindPP::twice(5), 10;
is Devel::BindPP::catfoo('hoge'), 'hogefoo';
is Devel::BindPP::twice_n(3.2), 6.4;
is Devel::BindPP::twice_u(5), 10;
isa_ok Devel::BindPP::do_bless([], 'OK'), 'OK';

# hash
is Devel::BindPP::hvref_fetch(+{a => 'b'}, 'a'), 'b';
is Devel::BindPP::hvref_fetch(+{a => 'b'}, 'c'), undef;

# array
is Devel::BindPP::avref_fetch([qw/a b c/], 1), 'b';
is Devel::BindPP::avref_fetch([qw/a b c/], 9), undef;

