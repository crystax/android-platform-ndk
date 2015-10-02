#include <locale>
#include <string>
#include <ios>

template <typename CharType>
class base_num_parse : public std::num_get<CharType>
{
public:
    base_num_parse()
        : std::num_get<CharType>(0)
    {}

protected:
    typedef typename std::num_get<CharType>::iter_type iter_type;
    typedef std::basic_string<CharType> string_type;
    typedef CharType char_type;

    virtual iter_type do_get(iter_type in, iter_type end, std::ios_base &ios,std::ios_base::iostate &err,unsigned short &val) const
    {
        return do_real_get(in,end,ios,err,val);
    }

private:
    template<typename ValueType>
    iter_type do_real_get(iter_type in,iter_type end,std::ios_base &ios,std::ios_base::iostate &err,ValueType &val) const
    {
        long double ret_val = 0;
        in = parse_currency<true>(in,end,ios,err,ret_val);
        if(!(err & std::ios_base::failbit))
            val = static_cast<ValueType>(ret_val);
        return in;
    }

    template<bool intl>
    iter_type parse_currency(iter_type in,iter_type end,std::ios_base &ios,std::ios_base::iostate &err,long double &val) const
    {
        std::locale loc = ios.getloc();
        int digits = std::use_facet<std::moneypunct<char_type,intl> >(loc).frac_digits();
        long double rval;
        in = std::use_facet<std::money_get<char_type> >(loc).get(in,end,intl,ios,err,rval);
        if(!(err & std::ios::failbit)) {
            while(digits > 0) {
                rval/=10;
                digits --;
            }
            val = rval;
        }
        return in;
    }
};

static base_num_parse<char> bb;

int main()
{
    base_num_parse<char> b;
    return 0;
}
