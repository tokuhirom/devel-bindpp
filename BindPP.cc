#include "bindpp.h"

#define XS_STATE(type, x) \
    INT2PTR(type, SvROK(x) ? SvIV(SvRV(x)) : SvIV(x))

#define XS_STRUCT2OBJ(sv, class, obj) \
    if (obj == NULL) { \
        sv_setsv(sv, &PL_sv_undef); \
    } else { \
        sv_setref_pv(sv, class, (void *) obj); \
    }


XS(XS_Devel__BindPP_twice) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice");
    }

    int n = c.arg_int(0);

    c.ret(0, pl::Int(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_n) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_n");
    }

    double n = c.arg_double(0);

    c.ret(0, pl::Double(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_u) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_u");
    }

    unsigned int n = c.arg_uint(0);

    c.ret(0, pl::UInt(n*2).mortal());
}

XS(XS_Devel__BindPP_catfoo) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(str)", "Devel::BindPP::catfoo");
    }

    const char* n = c.arg_str(0);
    std::string buf(n);
    buf += "foo";

    c.ret(0, pl::Str(buf).mortal());
}

XS(XS_hv_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(hv, str)", "Devel::BindPP::hv_fetch");
    }

    pl::Hash* hash = c.arg_hashref(0);
    const char* key = c.arg_str(1);

    pl::Reference * ret = hash->fetch(key);

    c.ret(0, ret);
}

XS(XS_av_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Array* array = c.arg_arrayref(0);
    const int key = c.arg_int(1);

    pl::Reference * ret = array->fetch(key);

    c.ret(0, ret);
}

XS(do_bless) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Reference* ref = c.arg_ref(0);
    const char * pkg = c.arg_str(1);
    ref->bless(pkg);

    c.ret(0, ref);
}

extern "C" {
    XS(boot_Devel__BindPP) {
        pl::BootstrapCtx bc;

        pl::Package pkg("Devel::BindPP");

        // Scalar
        pkg.add_method("twice", XS_Devel__BindPP_twice, __FILE__);
        pkg.add_method("catfoo", XS_Devel__BindPP_catfoo, __FILE__);
        pkg.add_method("twice_n", XS_Devel__BindPP_twice_n, __FILE__);
        pkg.add_method("twice_u", XS_Devel__BindPP_twice_u, __FILE__);
        pkg.add_method("do_bless", do_bless, __FILE__);

        // Hash
        pkg.add_method("hvref_fetch", XS_hv_fetch, __FILE__);

        // Array
        pkg.add_method("avref_fetch", XS_av_fetch, __FILE__);
    }
}

