use strict;
use warnings;
use Test::More tests => 20;
use Devel::BindPP;

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
    local $@;
    eval {
        Devel::BindPP::Scalar::to_c('ok');
    };
    like $@, qr{OK: 'ok'};
}
is Devel::BindPP::Scalar::twice_deref(\3), 6;
{
    my $a = 'a';
    is Devel::BindPP::Scalar::cats($a, 'b', 'c', 'de'), 'abcd';
}
ok !Devel::BindPP::Scalar::is_object([]);
ok Devel::BindPP::Scalar::is_object(bless []);
{
    is join(',', @{Devel::BindPP::Scalar::call_cv(sub { @_, qw/1 2 3/ })}), '3,2,1,4649';
    is join(',', @{Devel::BindPP::Scalar::call_cv(sub { @_, qw/1 2/ })}), '2,1,4649';
    is join(',', @{Devel::BindPP::Scalar::call_cv(sub { qw/1 2/ })}), '2,1';
    is Devel::BindPP::Scalar::call_cv_scalarcon(sub { 2 }), '4';
}
is Devel::BindPP::Scalar::len('hogehoge'), length('hogehoge');
is Devel::BindPP::Scalar::len(''), 0;
{
    my $a = 1;
    is Devel::BindPP::Scalar::refcnt($a), 1;
    my $b = \$a;
    is Devel::BindPP::Scalar::refcnt($a), 2;
}
