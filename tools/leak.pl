
use Devel::BindPP;
sub is { @_ }

{
    my $a = join ',', Devel::BindPP::Basic::mult();
    is $a, '4,9';
}
{
    is Devel::BindPP::Basic::mult2(), undef;
}
{
    my @a = Devel::BindPP::Basic::wantarray();
    is "@a", 'yes';
    my $a = Devel::BindPP::Basic::wantarray();
    is "$a", 'no';
}
is Devel::BindPP::Basic::FOO, 1981;
