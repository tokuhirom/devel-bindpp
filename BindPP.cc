#include "bindpp.h"

#define XS_STATE(type, x) \
    INT2PTR(type, SvROK(x) ? SvIV(SvRV(x)) : SvIV(x))

#define XS_STRUCT2OBJ(sv, class, obj) \
    if (obj == NULL) { \
        sv_setsv(sv, &PL_sv_undef); \
    } else { \
        sv_setref_pv(sv, class, (void *) obj); \
    }

XS(xs_p_new) {
    pl::Ctx c;

    char *self;
    Newx(self, 3, char);
    strcpy(self, "ok");

    pl::Pointer obj((void*)self, "Devel::BindPP::Pointer");
    c.ret(0, &obj);
}

XS(xs_p_get) {
    pl::Ctx c;

    pl::Pointer * p = c.arg(0)->as_pointer();

    char *self = p->extract<char>();
    pl::Str s(self);
    c.ret(0, &s);
}

XS(xs_p_destroy) {
    pl::Ctx c;

    pl::Pointer * p = c.arg(0)->as_pointer();
    Safefree(p->extract<char>());
    c.ret(0, pl::Boolean::yes());
}

XS(XS_Devel__BindPP_twice) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice");
    }

    int n = c.arg(0)->as_int()->to_c();

    c.ret(0, pl::Int(n*2).mortal());
}

XS(xs_twice_deref) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice");
    }

    int n = c.arg(0)->as_ref()->as_scalar()->as_int()->to_c();

    c.ret(0, pl::Int(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_n) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_n");
    }

    double n = c.arg(0)->as_double()->to_c();

    c.ret(0, pl::Double(n*2).mortal());
}

XS(XS_Devel__BindPP_twice_u) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(n)", "Devel::BindPP::twice_u");
    }

    unsigned int n = c.arg(0)->as_uint()->to_c();

    c.ret(0, pl::UInt(n*2).mortal());
}

XS(XS_Devel__BindPP_catfoo) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "Usage: %s(str)", "Devel::BindPP::catfoo");
    }

    const char* n = c.arg(0)->as_str()->to_c();
    std::string buf(n);
    buf += "foo";

    c.ret(0, pl::Str(buf).mortal());
}

XS(XS_hv_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(hv, str)", "Devel::BindPP::hv_fetch");
    }

    pl::Hash* hash = c.arg(0)->as_ref()->as_hash();
    const char* key = c.arg(1)->as_str()->to_c();

    pl::Reference * ret = hash->fetch(key);

    c.ret(0, ret);
}

XS(xs_av_fetch) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    const int key = c.arg(1)->as_int()->to_c();

    pl::Reference * ret = array->fetch(key);

    c.ret(0, ret);
}

XS(xs_av_push) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Scalar*s = c.arg(1);
    s->refcnt_inc();
    array->push(s);
}

XS(xs_av_unshift) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Int*i = c.arg(1)->as_int();
    array->unshift(i->to_c());
    c.return_true();
}

XS(xs_av_store) {
    pl::Ctx c;

    if (c.arg_len() != 3) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Int*i = c.arg(1)->as_int();
    pl::Scalar *val = c.arg(2);
    val->refcnt_inc();
    array->store(i->to_c(), val);
    c.return_true();
}

XS(xs_av_clear) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    array->clear();

    c.return_true();
}

XS(xs_av_undef) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    array->undef();

    c.return_true();
}

XS(xs_av_pop) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Scalar* s = array->pop();
    c.ret(0, s);
}

XS(xs_av_shift) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Scalar * s =  array->shift();
    c.ret(0, s);
}

XS(xs_av_len) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
        Perl_croak(aTHX_ "orz");
    }

    pl::Array* array = c.arg(0)->as_ref()->as_array();
    pl::Int i(array->len());
    c.ret(0, &i);
}

XS(do_bless) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Reference* ref = c.arg(0)->as_ref();
    const char * pkg = c.arg(1)->as_str()->to_c();
    ref->bless(pkg);

    c.ret(0, ref);
}

XS(XS_hv_exists) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Hash* hash = c.arg(0)->as_ref()->as_hash();
    const char * key = c.arg(1)->as_str()->to_c();

    pl::Boolean b(hash->exists(key));

    c.ret(0, &b);
}

XS(XS_hv_delete) {
    pl::Ctx c;

    if (c.arg_len() != 2) {
       Perl_croak(aTHX_ "Usage: %s(av, str)", "Devel::BindPP::av_fetch");
    }

    pl::Hash* hash = c.arg(0)->as_ref()->as_hash();
    const char * key = c.arg(1)->as_str()->to_c();

    pl::Reference * ref = hash->del(key);

    c.ret(0, ref);
}

XS(xs_refcnt_inc) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Scalar * s = c.arg(0);
    s->refcnt_inc();
    c.ret(0, s);
}

XS(xs_refcnt_dec) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Scalar * s = c.arg(0);
    s->refcnt_dec();
    c.ret(0, s);
}

XS(xs_to_c) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Str * s = c.arg(0)->as_str();
    Perl_croak(aTHX_ "OK: '%s'", s->to_c());
}

XS(xs_hv_store) {
    pl::Ctx c;

    if (c.arg_len() != 3) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    const char * key = c.arg(1)->as_str()->to_c();
    pl::Scalar * val = c.arg(2);

    pl::Reference *ref = h->store(key, val);
    ref->refcnt_inc();

    c.ret(0, ref);
}

XS(xs_hv_scalar) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    c.ret(0, h->scalar());
}

XS(xs_hv_undef) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
    h->undef();
    c.return_true();
}
XS(xs_hv_clear) {
    pl::Ctx c;

    if (c.arg_len() != 1) {
       Perl_croak(aTHX_ "orz");
    }

    pl::Hash * h = c.arg(0)->as_ref()->as_hash();
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
        s.add_method("to_c", xs_to_c, __FILE__);
        s.add_method("twice_deref", xs_twice_deref, __FILE__);

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
        a.add_method("avref_fetch", xs_av_fetch, __FILE__);
        a.add_method("push", xs_av_push, __FILE__);
        a.add_method("pop", xs_av_pop, __FILE__);
        a.add_method("len", xs_av_len, __FILE__);
        a.add_method("shift", xs_av_shift, __FILE__);
        a.add_method("unshift", xs_av_unshift, __FILE__);
        a.add_method("store", xs_av_store, __FILE__);
        a.add_method("clear", xs_av_clear, __FILE__);
        a.add_method("undef", xs_av_undef, __FILE__);

        // Pointer
        pl::Package p("Devel::BindPP::Pointer");
        p.add_method("new", xs_p_new, __FILE__);
        p.add_method("get", xs_p_get, __FILE__);
        p.add_method("DESTROY", xs_p_destroy, __FILE__);
    }
}

