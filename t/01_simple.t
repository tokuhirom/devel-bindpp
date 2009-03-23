use strict;
use warnings;
use Test::More tests => 23;

use Devel::BindPP;

# scalar
is Devel::BindPP::Scalar::twice(5), 10;
is Devel::BindPP::Scalar::catfoo('hoge'), 'hogefoo';
is Devel::BindPP::Scalar::twice_n(3.2), 6.4;
is Devel::BindPP::Scalar::twice_u(5), 10;
isa_ok Devel::BindPP::Scalar::do_bless([], 'OK'), 'OK';
{
    my $a = 'OK';
    Devel::BindPP::Scalar::refcnt_inc($a);
    Devel::BindPP::Scalar::refcnt_inc($a);
    Devel::BindPP::Scalar::refcnt_inc($a);
    is Internals::SvREFCNT($a), 4;
}
{
    my $b = 'OK';
    Devel::BindPP::Scalar::refcnt_inc($b);
    Devel::BindPP::Scalar::refcnt_inc($b);
    Devel::BindPP::Scalar::refcnt_inc($b);
    Devel::BindPP::Scalar::refcnt_dec($b);
    is Internals::SvREFCNT($b), 3;
}
{
    eval {
        Devel::BindPP::Scalar::c_str('ok');
    };
    like $@, qr{OK: 'ok'};
}

# hash
is Devel::BindPP::Hash::hvref_fetch(+{a => 'b'}, 'a'), 'b';
is Devel::BindPP::Hash::hvref_fetch(+{a => 'b'}, 'c'), undef;
ok !Devel::BindPP::Hash::exists(+{a => 'b'}, 'c');
ok Devel::BindPP::Hash::exists(+{a => 'b'}, 'a');
{
    my $a = +{a => 'b', 'c' => 'd'};
    is Devel::BindPP::Hash::delete($a, 'a'), 'b';
    is_deeply $a, {'c' => 'd'};
    is(Devel::BindPP::Hash::delete($a, 'a'), undef);
    is(Devel::BindPP::Hash::store($a, 'm', 'o'), 'o');
    is_deeply $a, +{'c' => 'd', 'm' => 'o'};
    is(Devel::BindPP::Hash::scalar($a), '2/8');
    Devel::BindPP::Hash::undef($a);
    is_deeply $a, +{};
    $a->{'b'} = 1;
    is_deeply $a, +{'b' => 1};
    Devel::BindPP::Hash::clear($a);
    is_deeply $a, +{};
}

# array
is Devel::BindPP::Array::avref_fetch([qw/a b c/], 1), 'b';
is Devel::BindPP::Array::avref_fetch([qw/a b c/], 9), undef;

