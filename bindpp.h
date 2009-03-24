#include <string>
#include <vector>

// TODO: use Newx instead of new
// TODO: use Safefree instaed of delete
// TODO: handle gv
// TODO: call cv by scalar context

extern "C" {
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#include "ppport.h"
#include <cstdarg>
};

namespace pl {
    class Str;
    class UInt;
    class Int;
    class Double;
    class Pointer;
    class Reference;
    class Hash;
    class Array;
    class Package;
    class Code;
    class Ctx;

    /**
     * abstract base class for perl values
     */
    class Value {
        friend class Ctx;
        friend class Reference;
        friend class Array;
        friend class Perl;
        friend class Hash;
        friend class Package;
        friend class Code;

    public:
        /**
         * dump value
         * @see sv_dump()
         */
        void dump() {
            sv_dump(val);
        }
        /**
         * increment the reference counter for this value
         * @see SvREFCNT_inc
         */
        void refcnt_inc() {
            SvREFCNT_inc(this->val);
        }
        /**
         * decrement the reference counter for this value
         * @see SvREFCNT_dec
         */
        void refcnt_dec() {
            SvREFCNT_dec(this->val);
        }
        /**
         * get a reference of this value
         */
        Reference* reference();
    protected:
        SV* val;
        Value() { }
        Value(SV* _v) {
            this->val = _v;
        }
    };

    /**
     * Scalar class(SV)
     */
    class Scalar : public Value {
        friend class Ctx;
        friend class Reference;
        friend class Array;
        friend class Perl;
        friend class Hash;
        friend class Package;
        friend class Code;

    public:
        /**
         * make this value as mortal.
         * mortal means "this variable is just a temporary.please remove after leave this context"
         */
        Scalar * mortal() {
            sv_2mortal(this->val);
            return this;
        }
        /**
         * serialize this object to real perl variable
         */
        SV * serialize() {
            return val;
        }
        /**
         * this variable is just a string.change the type
         */
        Str* as_str();
        /**
         * this variable is just a int.change the type
         */
        Int* as_int();
        /**
         * this variable is just a uint.change the type
         */
        UInt* as_uint();
        /**
         * this variable is just a double.change the type
         */
        Double* as_double();
        /**
         * this variable is just a pointer.change the type
         */
        Pointer* as_pointer();
        /**
         * this variable is just a reference.change the type
         */
        Reference* as_ref();
    protected:
        Scalar(SV* _v) : Value(_v) { }
    };

    /**
     * boolean class
     */
    class Boolean : public Scalar {
    public:
        Boolean(bool b) : Scalar(b ? &PL_sv_yes : &PL_sv_no) { }
        /**
         * get a 'yes' indicator variable
         * @see PL_sv_yes
         */
        static Boolean* yes();
        /**
         * get a 'no' indicator variable
         * @see PL_sv_no
         */
        static Boolean* no();
    };
    /**
     * integer(IV)
     */
    class Int : public Scalar {
        friend class Scalar;
    public:
        Int(int _i) : Scalar(newSViv(_i)) { }
        /**
         * convert to C level integer
         */
        int to_c() {
            return SvIV(this->val);
        }
    protected:
        Int(SV* _s) : Scalar(_s) { }
    };
    /**
     * unsigned integer(UV)
     */
    class UInt : public Scalar {
        friend class Scalar;
    public:
        UInt(unsigned int _i) : Scalar(newSVuv(_i)) { }
        /**
         * convert to C level unsigned integer
         */
        unsigned int to_c() {
            return SvUV(this->val);
        }
    protected:
        UInt(SV* _s) : Scalar(_s) { }
    };
    /**
     * double(NV)
     */
    class Double : public Scalar {
        friend class Scalar;
    public:
        Double(double _i) : Scalar(newSVnv(_i)) { }
        /**
         * convert to C level double
         */
        double to_c() {
            return SvNV(this->val);
        }
    protected:
        Double(SV* _s) : Scalar(_s) { }
    };
    /**
     * string(PV)
     */
    class Str : public Scalar {
        friend class Scalar;
    public:
        Str(std::string & _s) : Scalar(newSVpv(_s.c_str(), _s.length())) { }
        Str(const char* _s) : Scalar(newSVpv(_s, strlen(_s))) { }
        Str(const char* _s, int _n) : Scalar(newSVpv(_s, _n)) { }
        /**
         * convert to C level const char*
         */
        const char* to_c() {
            return SvPV_nolen(this->val);
        }
        /**
         * concat the string
         */
        void concat(const char* s, I32 len) {
            sv_catpvn(this->val, s, len);
        }
        void concat(const char* s) {
            sv_catpv(this->val, s);
        }
        void concat(Str* s) {
            sv_catsv(this->val, s->val);
        }
    protected:
        Str(SV* _s) : Scalar(_s) { }
    };

    /**
     * reference(RV)
     */
    class Reference : public Scalar {
        friend class Scalar;
        friend class Hash;
        friend class Array;
    public:
        /**
         * create a new reference with refcnt increment
         */
        static Reference * new_inc(Value* thing);
        /// bless the reference
        void bless(const char *pkg) {
            HV * stash = gv_stashpv(pkg, TRUE);
            sv_bless(this->val, stash);
        }
        /// dereference
        Hash * as_hash();
        /// dereference
        Array * as_array();
        /// dereference
        Scalar * as_scalar();
        /// dereference
        Code* as_code();
        /** is this object?
         * @see sv_isobject
         */
        bool is_object() {
            return sv_isobject(this->val);
        }
    protected:
        Reference(SV*v) : Scalar(v) { }
    };

    /**
     * %hash(HV)
     */
    class Hash : public Value {
        friend class Reference;
    public:
        Hash() : Value((SV*)newHV()) { }
        /// fetch the value of hash
        Reference * fetch(const char *key);
        /// exists(%a, $key)
        bool exists(const char*key) {
            return this->exists(key, strlen(key));
        }
        /// exists(%a, $key)
        bool exists(const char*key, I32 klen) {
            return hv_exists((HV*)this->val, key, klen);
        }
        /// remove the key in hash
        Reference* del(const char*key) {
            return this->del(key, strlen(key));
        }
        /// remove the key in hash
        Reference* del(const char*key, I32 klen);

        /// store value to hash
        Reference* store(const char*key, Scalar*value) {
            return this->store(key, strlen(key), value);
        }
        /// store value to hash
        Reference* store(const char*key, I32 klen, Scalar*value);
        /// Evaluates the hash in scalar context and returns the result.
        Scalar* scalar();
        /// Undefines the hash.
        void undef();
        /// Clears a hash, making it empty.
        void clear();
    protected:
        Hash(HV* _h) : Value((SV*)_h) { }
    };

    /**
     * array(AV)
     */
    class Array : public Value {
        friend class Reference;
    public:
        Array() : Value((SV*)newAV()) { }
        /// push the value
        void push(Value v) {
            this->push(&v);
        }
        /// push the value
        void push(Value * v) {
            v->refcnt_inc();
            av_push((AV*)this->val, v->val);
        }
        /**
          * Unshift the given number of "undef" values onto the beginning
          * of the array.
          */
        void unshift(Int &i) {
            this->unshift(i.to_c());
        }
        void unshift(I32 i) {
            av_unshift((AV*)this->val, i);
        }
        /// pops value from the array
        Scalar * pop();
        /// shifts value from the array
        Scalar * shift();
        /// fetch value from the array
        Reference * fetch(I32 key);

        /// len returns highest index in array
        I32 len() {
            return av_len((AV*)this->val);
        }
        /// size() returns size of array(= len()+1)
        U32 size() {
            return this->len() + 1;
        }

        /// store values to array
        Scalar * store(I32 key, Scalar* v);
        /// Clears an array, making it empty.
        void clear() {
            av_clear((AV*)this->val);
        }
        /// Undefines this value
        void undef() {
            av_undef((AV*)this->val);
        }
        /// Pre-extend an array.
        void extend(I32 n) {
            av_extend((AV*)this->val, n);
        }
    protected:
        Array(AV* _a) : Value((SV*)_a) { }
    };

    /**
     * XSUB context class
     */
    class Ctx {
    public:
        Ctx();
        ~Ctx();
        /// length of arguments
        I32 arg_len() {
            return (I32)(PL_stack_sp - mark);
        }
        /// get the argument indexed by n
        Scalar* arg(int n) {
            Scalar*s = new Scalar(fetch_stack(n));
            this->register_allocated(s);
            return s;
        }
        /// return the one scalar value
        void ret(Scalar s) {
            this->ret(0, &s);
        }
        void ret(Scalar* s) {
            this->ret(0, s);
        }
        void ret(int n, Scalar* s) {
            this->ret(n, s ? s->serialize() : &PL_sv_undef);
        }
        /// same as perl level wantarray()
        bool wantarray() {
            return GIMME_V & G_ARRAY ? true : false;
        }
        /// return multiple values
        void ret(Array* ary) {
            unsigned int size = ary->size();
            if (size != 0) {
                SV** sp = PL_stack_sp;
                if ((unsigned int)(PL_stack_max - sp) < size) {
                    sp = stack_grow(sp, sp, size);
                }

                for (unsigned int i=0; i < size; ++i) {
                    Scalar * s = ary->fetch(i);
                    PL_stack_base[ax++] = s->val;
                }
                ax--;
            } else {
                this->return_undef();
            }
        }
        /// return true value
        void return_true() {
            this->ret(0, &PL_sv_yes);
        }
        /// return undef value
        void return_undef() {
            this->ret(0, &PL_sv_undef);
        }
        /**
          * register the allocated Value. these objects delete
          * when leave this context.
          * Note: 'Value' is delete, but Value->val is not delete!
          */
        void register_allocated(Value* v) {
            allocated.push_back(v);
        }
    protected:
        /**
          * fetch the top 'n' of stack
          * @see ST(n)
          */
        SV* fetch_stack(int n) {
            return PL_stack_base[this->ax + n];
        }
        void ret(int n, SV* s) {
            PL_stack_base[ax + n] = s;
        }
        I32 ax;
        SV ** mark;
        std::vector<Value*> allocated;
    };
    std::vector<Ctx*> ctxstack;
    Ctx::Ctx() {
        // same as dAXMARK
        this->ax = *PL_markstack_ptr + 1;
        --PL_markstack_ptr;
        this->mark = PL_stack_base + ax - 1;

        ctxstack.push_back(this);
    }
    Ctx::~Ctx() {
        std::vector<Value*>::iterator iter;
        for (iter = allocated.begin(); iter != allocated.end(); iter++) {
            delete *iter;
        }

        PL_stack_sp = PL_stack_base + ax;

        ctxstack.pop_back();
    }

    /**
     * current context registrar
     */
    class CurCtx {
    public:
        static Ctx * get() {
            return ctxstack[ctxstack.size()-1];
        }
    };

    Reference * Reference::new_inc(Value* thing) {
        Reference* ref = new Reference(newRV_inc(thing->val));
        CurCtx::get()->register_allocated(ref);
        return ref;
    }

    Reference * Hash::fetch(const char* key) {
        // SV**    hv_fetch(HV* tb, const char* key, I32 klen, I32 lval)
        SV ** v = hv_fetch((HV*)this->val, key, strlen(key), 0);
        if (v) {
            Reference * ref = new Reference(*v);
            CurCtx::get()->register_allocated(ref);
            return ref;
        } else {
            return NULL;
        }
    }
    Reference * Array::fetch(I32 key) {
        SV ** v = av_fetch((AV*)this->val, key, 0);
        if (v) {
            Reference * ref = new Reference(*v);
            CurCtx::get()->register_allocated(ref);
            return ref;
        } else {
            return NULL;
        }
    }
    Scalar * Array::pop() {
        SV* v = av_pop((AV*)this->val);
        Scalar *s = new Scalar(v);
        CurCtx::get()->register_allocated(s);
        return s;
    }
    Scalar * Array::shift() {
        SV* v = av_shift((AV*)this->val);
        Scalar *s = new Scalar(v);
        CurCtx::get()->register_allocated(s);
        return s;
    }
    Scalar * Array::store(I32 key, Scalar* _v) {
        _v->refcnt_inc();
        SV** v = av_store((AV*)this->val, key, _v->val);
        if (v) {
            Reference * ref = new Reference(*v);
            CurCtx::get()->register_allocated(ref);
            return ref;
        } else {
            return NULL;
        }
    }

    /**
     * package class
     */
    class Package {
    public:
        Package(std::string _pkg) {
            pkg = _pkg;
            stash = gv_stashpvn(pkg.c_str(), pkg.length(), TRUE);
        }
        /**
         * install xsub
         * @see newXS
         */
        void add_method(const char*name, XSUBADDR_t func, const char *file) {
            newXS((pkg + "::" + name).c_str(), func, file);
        }
        /**
         * add new const sub
         * same as sub FOO() { 1 }
         */
        void add_constant(const char *name, Value * val) {
            this->add_constant(name, val->val);
        }
        void add_constant(const char *name, Value val) {
            this->add_constant(name, val.val);
        }
    protected:
        void add_constant(const char *name, SV* val) {
            newCONSTSUB(stash, name, val);
        }
    private:
        std::string pkg;
        HV * stash;
    };

    /**
     * special context class for bootstrap_* method
     */
    class BootstrapCtx : public Ctx {
    public:
        BootstrapCtx() : Ctx() {
            xs_version_bootcheck();
        }
        ~BootstrapCtx() {
            PL_stack_base[this->ax] = &PL_sv_yes;
            PL_stack_sp = PL_stack_base + this->ax;
        }
    protected:
        // XS_VERSION_BOOTCHECK
        void xs_version_bootcheck() {
            SV *_sv;
            const char *vn = NULL, *module = SvPV_nolen_const(ST(0));
            if (this->arg_len() >= 2) {
                /* version supplied as bootstrap arg */
                _sv = PL_stack_base[ax+1];
            } else {
                /* XXX GV_ADDWARN */
                _sv = get_sv(Perl_form(aTHX_ "%s::%s", module,
                        vn = "XS_VERSION"), FALSE);
                if (!_sv || !SvOK(_sv))
                _sv = get_sv(Perl_form(aTHX_ "%s::%s", module,
                            vn = "VERSION"), FALSE);
            }
            if (_sv) {
                SV *xssv = Perl_newSVpv(aTHX_ XS_VERSION, 0);
                xssv = new_version(xssv);
                if ( !sv_derived_from(_sv, "version") ) {
                    _sv = new_version(_sv);
                }
                if ( vcmp(_sv,xssv) ) {
                    Perl_croak(aTHX_ "%s object version %"SVf" does not match %s%s%s%s %"SVf,
                        module, SVfARG(vstringify(xssv)),
                        vn ? "$" : "", vn ? module : "", vn ? "::" : "",
                        vn ? vn : "bootstrap parameter", SVfARG(vstringify(_sv)));
                }
            }
        }
    };

    /**
     * coderef(CV)
     */
    class Code : public Scalar {
    public:
        Code(SV * _s) : Scalar(_s) { }
        /// call the coderef by list context
        void call(Array * args, Array* retval) {
            SV **sp = PL_stack_sp;

            push_scope(); // ENTER
            save_int((int*)&PL_tmps_floor); // SAVETMPS
            PL_tmps_floor = PL_tmps_ix;

            if (++PL_markstack_ptr == PL_markstack_max) { // PUSHMARK(SP);
                markstack_grow();
            }
            *PL_markstack_ptr = (I32)((sp) - PL_stack_base);

            for (int i =0; i < args->len()+1; i++) {
                if (PL_stack_max - sp < 1) { // EXTEND()
                    // optimize?
                    sp = stack_grow(sp, sp, 1);
                }
                *++sp = args->pop()->val; // XPUSHs
            }
            PL_stack_sp = sp; // PUTBACK

            int count = call_sv(this->val, G_ARRAY);

            sp = PL_stack_sp; // SPAGAIN

            for (int i=0; i<count; i++) {
                Scalar * s = new Scalar(newSVsv(*sp--));
                CurCtx::get()->register_allocated(s);
                retval->store(i, s);
            }

            PL_stack_sp = sp; // PUTBACK
            if (PL_tmps_ix > PL_tmps_floor) { // FREETMPS
                free_tmps();
            }
            pop_scope(); // LEAVE
        }
        /// call the coderef by scalar context
        void call(Array * args, Scalar** retval) {
            SV **sp = PL_stack_sp;

            push_scope(); // ENTER
            save_int((int*)&PL_tmps_floor); // SAVETMPS
            PL_tmps_floor = PL_tmps_ix;

            if (++PL_markstack_ptr == PL_markstack_max) { // PUSHMARK(SP);
                markstack_grow();
            }
            *PL_markstack_ptr = (I32)((sp) - PL_stack_base);

            for (int i =0; i < args->len()+1; i++) {
                if (PL_stack_max - sp < 1) { // EXTEND()
                    // optimize?
                    sp = stack_grow(sp, sp, 1);
                }
                *++sp = args->pop()->val; // XPUSHs
            }
            PL_stack_sp = sp; // PUTBACK

            int count = call_sv(this->val, G_SCALAR);

            sp = PL_stack_sp; // SPAGAIN

            if (count != 0) {
                *retval = new Scalar(newSVsv(*sp--));
                CurCtx::get()->register_allocated(*retval);
            }

            PL_stack_sp = sp; // PUTBACK
            if (PL_tmps_ix > PL_tmps_floor) { // FREETMPS
                free_tmps();
            }
            pop_scope(); // LEAVE
        }
    };

    /**
     * pointer class(blessed object, contains pointer for C struct/class/etc.)
     */
    class Pointer : public Scalar {
    public:
        Pointer(SV* s) : Scalar(s) { }
        /// create a scalar from void* pointer
        Pointer(void* _ptr, const char* klass) : Scalar(sv_newmortal()) {
            if (_ptr == NULL) {
                sv_setsv(this->val, &PL_sv_undef);
            } else {
                sv_setref_pv(this->val, klass, _ptr);
            }
        }

        /**
         * get the pointer from scalar
         */
        template <class T>
        T extract() {
            return INT2PTR(T, SvROK(this->val) ? SvIV(SvRV(this->val)) : SvIV(this->val));
        }
    };

    /**
     * utility functions
     */
    class Perl {
    public:
        // static Hash* get_stash(Str* name);
//      template<class U>
//      static void* alloc(int size) {
//          void *buf;
//          Newx(buf, size, U);
//          return buf;
//      }
    };

    Reference * Value::reference() {
        return Reference::new_inc(this);
    }

    Str* Scalar::as_str() {
        if (SvPOK(this->val)) {
            Str * s = new Str(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a string",
                "Devel::BindPP",
                "sv");
        }
    }
    Pointer* Scalar::as_pointer() {
        if (SvROK(this->val)) {
            Pointer * s = new Pointer(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a pointer",
                "Devel::BindPP",
                "sv");
        }
    }
    Int* Scalar::as_int() {
        if (SvIOKp(this->val)) {
            Int * s = new Int(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a int",
                "Devel::BindPP",
                "sv");
        }
    }
    UInt* Scalar::as_uint() {
        if (SvIOK(this->val)) {
            UInt * s = new UInt(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a uint",
                "Devel::BindPP",
                "sv");
        }
    }
    Double* Scalar::as_double() {
        if (SvNOK(this->val)) {
            Double * s = new Double(this->val);
            CurCtx::get()->register_allocated(s);
            return s;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a double",
                "Devel::BindPP",
                "sv");
        }
    }
    Reference* Scalar::as_ref() {
        if (SvROK(this->val)) {
            Reference * obj = new Reference(this->val);
            CurCtx::get()->register_allocated(obj);
            return obj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a reference",
                "Devel::BindPP",
                "sv");
        }
    }
    /*
    Hash* Perl::get_stash(Str * name) {
        HV * stash = gv_stashpv(name->to_c(), 0);
        Hash * h = new Hash(stash);
        CurCtx::get()->register_allocated(h);
        return h;
    }
    */

    Hash * Reference::as_hash() {
        if (SvROK(this->val) && SvTYPE(SvRV(this->val))==SVt_PVHV) {
            HV* h = (HV*)SvRV(this->val);
            Hash * hobj = new Hash(h);
            CurCtx::get()->register_allocated(hobj);
            return hobj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a hash reference",
                "Devel::BindPP",
                "hv");
        }
    }
    Array * Reference::as_array() {
        SV* v = this->val;
        if (SvROK(v) && SvTYPE(SvRV(v))==SVt_PVAV) {
            AV* a = (AV*)SvRV(v);
            Array * obj = new Array(a);
            CurCtx::get()->register_allocated(obj);
            return obj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a array reference",
                "Devel::BindPP",
                "av");
        }
    }
    Scalar * Reference::as_scalar() {
        SV* v = this->val;
        if (v && SvROK(v)) {
            SV* a = (SV*)SvRV(v);
            Scalar * obj = new Scalar(a);
            CurCtx::get()->register_allocated(obj);
            return obj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a array reference",
                "Devel::BindPP",
                "sv");
        }
    }
    Code * Reference::as_code() {
        SV* v = this->val;
        if (v && SvROK(v)) {
            SV* a = (SV*)SvRV(v);
            Code * obj = new Code(a);
            CurCtx::get()->register_allocated(obj);
            return obj;
        } else {
            Perl_croak(aTHX_ "%s: %s is not a array reference",
                "Devel::BindPP",
                "sv");
        }
    }

    Reference* Hash::del(const char*key, I32 klen) {
        Reference * ref = new Reference(hv_delete((HV*)this->val, key, klen, 0));
        CurCtx::get()->register_allocated(ref);
        return ref;
    }
    Reference* Hash::store(const char*key, I32 klen, Scalar*value) {
        value->refcnt_inc();
        SV ** s = hv_store((HV*)this->val, key, klen, value->val, 0);
        assert(s && SvROK(*s));
        Reference * ref = new Reference(*s);
        CurCtx::get()->register_allocated(ref);
        return ref;
    }
    Scalar* Hash::scalar() {
        Scalar*s = new Scalar(hv_scalar((HV*)this->val));
        CurCtx::get()->register_allocated(s);
        return s;
    }
    void Hash::undef() {
        hv_undef((HV*)this->val);
    }
    void Hash::clear() {
        hv_clear((HV*)this->val);
    }
    Boolean* Boolean::yes() {
        Boolean* s = new Boolean(true);
        CurCtx::get()->register_allocated(s);
        return s;
    }
    Boolean* Boolean::no() {
        Boolean* s = new Boolean(false);
        CurCtx::get()->register_allocated(s);
        return s;
    }
};