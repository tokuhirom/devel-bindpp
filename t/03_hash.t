use strict;
use warnings;
use Test::More tests => 12;

use Devel::BindPP;

# hash
is Devel::BindPP::Hash::hvref_fetch(+{a => 'b'}, 'a'), 'b';
is Devel::BindPP::Hash::hvref_fetch(+{a => 'b'}, 'c'), undef;
ok !Devel::BindPP::Hash::exists(+{a => 'b'}, 'c');
ok Devel::BindPP::Hash::exists(+{a => 'b'}, 'a');
{
    my $a = +{a => 'b', 'c' => 'd'};
    is Devel::BindPP::Hash::delete($a, 'a'), 'b';
    is_deeply $a, {'c' => 'd'}, 'removed';
    is(Devel::BindPP::Hash::delete($a, 'a'), undef);
    Devel::BindPP::Hash::store($a, 'm', 'o');
    is_deeply $a, +{'c' => 'd', 'm' => 'o'};
    is(Devel::BindPP::Hash::scalar($a), '2/8');
    Devel::BindPP::Hash::undef($a);
    is_deeply $a, +{};
    $a->{'b'} = 1;
    is_deeply $a, +{'b' => 1};
    Devel::BindPP::Hash::clear($a);
    is_deeply $a, +{}, 'clear';
}

