<?
// Copyright 2014, Tera Insights LLC. All Rights Reserved.

function TIME() {
    $methods = [];
    $constructors = [];
    $functions = [];
    $bin_operators = [];

    $globalContent = "";
?>

class TIME {
private:
    int32_t nMillis;

    static constexpr int32_t LEAP_START = 86399000;
    static constexpr int32_t MAX_TIME = 863999999;
    static constexpr int32_t INVALID_TIME = std::numeric_limits<int32_t>::min();

    static constexpr int32_t SECONDS_TO_MILLIS = 1000;
    static constexpr int32_t MINUTES_TO_MILLIS = SECONDS_TO_MILLIS * 60;
    static constexpr int32_t HOURS_TO_MILLIS = MINUTES_TO_MILLIS * 60;

    static constexpr int32_t MILLIS_PER_SECOND = 1000;
    static constexpr int32_t SECONDS_PER_MINUTE = 60;
    static constexpr int32_t MINUTES_PER_HOUR = 60;

    static constexpr int32_t MillisFromFacets(const int32_t h, const int32_t m, const int32_t s, const int32_t ms);

public:
<?  $constructors[] = [[], true];  ?>
    constexpr TIME(void);

<?  $constructors[] = [['BASE::INT'], true];  ?>
    constexpr TIME( const int32_t _millis );

<?  $constructors[] = [['BASE::NULL'], true];  ?>
    constexpr TIME( const GrokitNull & null );

<?  $constructors[] = [['BASE::INT', 'BASE::INT', 'BASE::INT', 'BASE::INT'], true];  ?>
    constexpr TIME(
        const int32_t hours,
        const int32_t minutes,
        const int32_t seconds,
        const int32_t millis
    );


<?  $methods[] = [ 'hour', [], 'BASE::BYTE', true ];  ?>
    // Get the hours portion of the time
    constexpr int8_t hour(void) const;
    constexpr int8_t hours(void) const;
<?  $methods[] = [ 'minute', [], 'BASE::BYTE', true ];  ?>
    // Get the minutes portion of the time
    constexpr int8_t minute(void) const;
    constexpr int8_t minutes(void) const;
<?  $methods[] = [ 'second', [], 'BASE::BYTE', true ];  ?>
    // Get the seconds portion of the time
    constexpr int8_t second(void) const;
    constexpr int8_t seconds(void) const;
<?  $methods[] = [ 'milli', [], 'BASE::SMALLINT', true ];  ?>
    // Get the milliseconds portion of the time
    constexpr int16_t milli(void) const;
    constexpr int16_t millis(void) const;

<?  $methods[] = ['as_hours', [], 'BASE::INT', true ];  ?>
    // Get the time in hours since the beginning of the day
    constexpr int32_t as_hours(void) const;
<?  $methods[] = ['as_minutes', [], 'BASE::INT', true ];  ?>
    // Get the time in minutes since the beginning of the day
    constexpr int32_t as_minutes(void) const;
<?  $methods[] = ['as_seconds', [], 'BASE::INT', true]; ?>
    // Get the time in seconds since the beginning of the day
    constexpr int32_t as_seconds(void) const;
<?  $methods[] = ['as_millis', [], 'BASE::INT', true]; ?>
    // Get the time in milliseconds since the beginning of the day
    constexpr int32_t as_millis(void) const;


    constexpr bool is_valid(void) const;
    constexpr bool is_null(void) const;

    // Comparison operators
<?  $bin_operators[] = '==';  ?>
    constexpr bool operator ==( const TIME & o ) const;
<?  $bin_operators[] = '!=';  ?>
    constexpr bool operator !=( const TIME & o ) const;
<?  $bin_operators[] = '>';  ?>
    constexpr bool operator >( const TIME & o ) const;
<?  $bin_operators[] = '<';  ?>
    constexpr bool operator <( const TIME & o ) const; //>
<?  $bin_operators[] = '>=';  ?>
    constexpr bool operator >=( const TIME & o ) const;
<?  $bin_operators[] = '<=';  ?>
    constexpr bool operator <=( const TIME & o ) const; //>

    // Addition and subtraction of time intervals
<?  $bin_operators[] = '+';  ?>
    constexpr TIME operator +( const TIME & o ) const;
<?  $bin_operators[] = '-';  ?>
    constexpr TIME operator -( const TIME & o ) const;
};

inline
constexpr int32_t TIME :: MillisFromFacets(const int32_t h, const int32_t m, const int32_t s, const int32_t ms) {
    return (h * HOURS_TO_MILLIS) + (m * MINUTES_TO_MILLIS) + (s * SECONDS_TO_MILLIS) + ms;
}

inline
constexpr TIME :: TIME(void) : nMillis(INVALID_TIME)
{ }

inline
constexpr TIME :: TIME( const int32_t _millis ) : nMillis(_millis)
{ }

inline
constexpr TIME :: TIME( const int32_t h, const int32_t m, const int32_t s, const int32_t ms ) :
    nMillis(MillisFromFacets(h, m, s, ms))
{ }

inline
constexpr int32_t TIME :: as_hours(void) const {
    return nMillis / HOURS_TO_MILLIS;
}

inline
constexpr int8_t TIME :: hour(void) const {
    return as_hours();
}

inline
constexpr int8_t TIME :: hours(void) const {
    return hour();
}

inline
constexpr int32_t TIME :: as_minutes(void) const {
    return nMillis / MINUTES_TO_MILLIS;
}

inline
constexpr int8_t TIME :: minute(void) const {
    return as_minutes() % MINUTES_PER_HOUR;
}

inline
constexpr int8_t TIME :: minutes(void) const {
    return minute();
}

inline
constexpr int32_t TIME :: as_seconds(void) const {
    return nMillis / SECONDS_TO_MILLIS;
}

inline
constexpr int8_t TIME :: second(void) const {
    return (LEAP_START > nMillis) ? as_seconds() % SECONDS_PER_MINUTE : 60;
}

inline
constexpr int8_t TIME :: seconds(void) const {
    return second();
}

inline
constexpr int32_t TIME :: as_millis(void) const {
    return nMillis;
}

inline
constexpr int16_t TIME :: milli(void) const {
    return as_millis() % MILLIS_PER_SECOND;
}

inline
constexpr int16_t TIME :: millis(void) const {
    return milli();
}

inline
constexpr bool TIME :: is_valid(void) const {
    return nMillis >= 0 && MAX_TIME >= nMillis;
}

inline
constexpr bool TIME :: is_null(void) const {
    return nMillis == INVALID_TIME;
}

inline
constexpr bool TIME :: operator ==( const TIME & o ) const {
    return nMillis == o.nMillis;
}

inline
constexpr bool TIME :: operator !=( const TIME & o ) const {
    return nMillis != o.nMillis;
}

inline
constexpr bool TIME :: operator >( const TIME & o ) const {
    return nMillis > o.nMillis;
}

inline
constexpr bool TIME :: operator <( const TIME & o ) const { //>
    return nMillis < o.nMillis; //>
}

inline
constexpr bool TIME :: operator >=( const TIME & o ) const {
    return nMillis >= o.nMillis;
}

inline
constexpr bool TIME :: operator <=( const TIME & o ) const { //>
    return nMillis <= o.nMillis; //>
}

inline
constexpr TIME TIME :: operator +( const TIME & o ) const {
    return TIME(nMillis + o.nMillis);
}

inline
constexpr TIME TIME :: operator -( const TIME & o ) const {
    return TIME(nMillis - o.nMillis);
}

<?
    return [
        'kind'              => 'TYPE',
        'system_headers'    => [ 'cinttypes', 'limits' ],
        'user_headers'      => [ 'Config.h' ],
        'binary_operators'  => $bin_operators,
        'global_content'    => $globalContent,
        'constructors'      => $constructors,
        'methods'           => $methods,
        'functions'         => $functions,
        'describe_json'     => DescribeJson('time', DescribeJsonStatic(['format' => 'HH:mm:ss.SSS'])),
        'extras'            => [ 'size.bytes' => 4 ],
    ];
}
?>
