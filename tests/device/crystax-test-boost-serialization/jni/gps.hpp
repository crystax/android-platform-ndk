#ifndef GPS_HPP_7D5AF29629F64210BE00F3AF697BA650
#define GPS_HPP_7D5AF29629F64210BE00F3AF697BA650

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/////////////////////////////////////////////////////////////
// gps coordinate
//
// illustrates serialization for a simple type
//
class gps_position
{
private:
    friend class boost::serialization::access;
    friend std::ostream &operator<<(std::ostream &, gps_position const &);
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
        ar & degrees;
        ar & minutes;
        ar & seconds;
    }
    int degrees;
    int minutes;
    float seconds;
public:
    gps_position(){}
    gps_position(int d, int m, float s) :
        degrees(d), minutes(m), seconds(s)
    {}

    bool operator==(gps_position const &g) const
    {
        return degrees == g.degrees && minutes == g.minutes && seconds == g.seconds;
    }

    bool operator!=(gps_position const &g) const
    {
        return !(*this == g);
    }
};

void save(gps_position const &g);
void load(gps_position &g);

#endif // GPS_HPP_7D5AF29629F64210BE00F3AF697BA650
