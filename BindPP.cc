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

    const char* n = c.arg_scalar(0)->as_str()->c_str();
    std::string buf(n);
    buf += "foo";

    c.ret(0, pl::Str(buf).mortal());
}

XS(XS_hv_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(hv, str)", "Devel::BindPP::hv_fetch");
    }

    pl::Hash* hash = c.arg_ref(0)->as_hash();
    const char* key = c.arg_scalar(1)->as_str()->c_str();

    pl::Reference * ret = hash->fetch(key);

    c.ret(0, ret);
}

XS(XS_av_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Array* array = c.arg_ref(0)->as_array();
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
    const char * pkg = c.arg_scalar(1)->as_str()->c_str();
    ref->bless(pkg);

    c.ret(0, ref);
}

XS(XS_hv_exists) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Hash* hash = c.arg_ref(0)->as_hash();
    const char * key = c.arg_scalar(1)->as_str()->c_str();

    pl::Boolean b(hash->exists(key));

    c.ret(0, &b);
}

XS(XS_hv_delete) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Hash* hash = c.arg_ref(0)->as_hash();
    const char * key = c.arg_scalar(1)->as_str()->c_str();

    pl::Reference * ref = hash->del(key);

    c.ret(0, ref);
}

XS(xs_refcnt_inc) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Scalar * s = c.arg_scalar(0);
    s->refcnt_inc();
    c.ret(0, s);
}

XS(xs_refcnt_dec) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Scalar * s = c.arg_scalar(0);
    s->refcnt_dec();
    c.ret(0, s);
}

XS(xs_c_str) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Str * s = c.arg_scalar(0)->as_str();
    Perl_croak(aTHX_ "OK: '%s'", s->c_str());
}

XS(xs_hv_store) {
    pl::Ctx c;

    if (c.arg_len() != 3) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg_ref(0)->as_hash();
    const char * key = c.arg_scalar(1)->as_str()->c_str();
    pl::Scalar * val = c.arg_scalar(2);

    pl::Reference *ref = h->store(key, val);
    ref->refcnt_inc();

    c.ret(0, ref);
}

XS(xs_hv_scalar) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg_ref(0)->as_hash();
    c.ret(0, h->scalar());
}

XS(xs_hv_undef) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg_ref(0)->as_hash();
    h->undef();
    c.ret(0, pl::Boolean::yes());
}
XS(xs_hv_clear) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg_ref(0)->as_hash();
    h->clear();
    c.ret(0, pl::Boolean::yes());
}

extern "C" {
    XS(boot_Devel__BindPP) {
        pl::BootstrapCtx bc;

        pl::Package s("Devel::BindPP::Scalar");
        s.add_method("twice", XS_Devel__BindPP_twice, __FILE__);
        s.add_method("catfoo", XS_Devel__BindPP_catfoo, __FILE__);
        s.add_method("twice_n", XS_Devel__BindPP_twice_n, __FILE__);
        s.add_method("twice_u", XS_Devel__BindPP_twice_u, __FILE__);
        s.add_method("do_bless", do_bless, __FILE__);
        s.add_method("refcnt_inc", xs_refcnt_inc, __FILE__);
        s.add_method("refcnt_dec", xs_refcnt_dec, __FILE__);
        s.add_method("c_str", xs_c_str, __FILE__);

        // Hash
        pl::Package h("Devel::BindPP::Hash");
        h.add_method("hvref_fetch", XS_hv_fetch, __FILE__);
        h.add_method("exists", XS_hv_exists, __FILE__);
        h.add_method("delete", XS_hv_delete, __FILE__);
        h.add_method("store", xs_hv_store, __FILE__);
        h.add_method("scalar", xs_hv_scalar, __FILE__);
        h.add_method("undef", xs_hv_undef, __FILE__);
        h.add_method("clear", xs_hv_clear, __FILE__);

        // Array
        pl::Package a("Devel::BindPP::Array");
        a.add_method("avref_fetch", XS_av_fetch, __FILE__);
    }
}

