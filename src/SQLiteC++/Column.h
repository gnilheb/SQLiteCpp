/**
 * @file  Column.h
 * @brief Encapsulation of a Column in a Row of the result.
 *
 * Copyright (c) 2012-2013 Sebastien Rombauts (sebastien.rombauts@gmail.com)
 *
 * Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
 * or copy at http://opensource.org/licenses/MIT)
 */
#pragma once

#include <sqlite3.h>
#include "Exception.h"
#include "Statement.h"

namespace SQLite
{


/**
 * @brief Encapsulation of a Column in a Row of the result.
 *
 *  A Column is a particular field of SQLite data in the current row of result
 * of the Statement : it points to a single cell.
 *
 * Its value can be expressed as a text, and, when applicable, as a numeric
 * (integer or floting point) or a binary blob.
 */
class Column
{
public:
    /**
     * @brief Encapsulation of a Column in a Row of the result.
     *
     * @param[in] aStmtPtr  Shared pointer to the prepared SQLite Statement Object.
     * @param[in] aIndex    Index of the column in the row of result
     */
    Column(Statement::Ptr& aStmtPtr, int aIndex)    throw(); // nothrow
    /// Simple destructor
    virtual ~Column(void)                           throw(); // nothrow
    // default copy constructor and asignement operator are perfectly suited :
    // they copy the Statement::Ptr which in turn increments the reference counter.
    
    /// Return the integer value of the column.
    int             getInt   (void) const throw(); // nothrow
    /// Return the 64bits integer value of the column.
    sqlite3_int64   getInt64 (void) const throw(); // nothrow
    /// Return the double (64bits float) value of the column.
    double          getDouble(void) const throw(); // nothrow
    /// Return a pointer to the text value (NULL terminated string) of the column.
    /// Warning, the value pointed at is only valid while the statement is valid (ie. not finalized),
    /// thus you must copy it before using it beyond its scope (to a std::string for instance).
    const char*     getText  (void) const throw(); // nothrow
    const void*     getBlob  (void) const throw(); // nothrow
    
    /**
     * @brief Return the type of the value of the column
     *
     * Return either SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_TEXT, SQLITE_BLOB, or SQLITE_NULL.
     *
     * @warning After a type conversion (by a call to a getXxx on a Column of a Yyy type),
     *          the value returned by sqlite3_column_type() is undefined.
     */
    int getType(void) const throw(); // nothrow
    
    /// Test if the column is an integer type value (meaningfull only before any conversion)
    inline bool isInteger(void) const throw() // nothrow
    {
        return (SQLITE_INTEGER == getType());
    }
    /// Test if the column is a floting point type value (meaningfull only before any conversion)
    inline bool isFloat(void) const throw() // nothrow
    {
        return (SQLITE_FLOAT == getType());
    }
    /// Test if the column is a text type value (meaningfull only before any conversion)
    inline bool isText(void) const throw() // nothrow
    {
        return (SQLITE_TEXT == getType());
    }
    /// Test if the column is a binary blob type value (meaningfull only before any conversion)
    inline bool isBlob(void) const throw() // nothrow
    {
        return (SQLITE_BLOB == getType());
    }
    /// Test if the column is NULL (meaningfull only before any conversion)
    inline bool isNull(void) const throw() // nothrow
    {
        return (SQLITE_NULL == getType());
    }
    
    /**
     * @brief Return the number of bytes used by the text value of the column
     *
     * Return either :
     * - size in bytes (not in characters) of the string returned by getText() without the '\0' terminator
     * - size in bytes of the string representation of the numerical value (integer or double)
     * - size in bytes of the binary blob returned by getBlob()
     * - 0 for a NULL value
     */
    int getBytes(void) const throw();

    /// Inline cast operator to int
    inline operator int() const
    {
        return getInt();
    }
    /// Inline cast operator to 64bits integer
    inline operator sqlite3_int64() const
    {
        return getInt64();
    }
    /// Inline cast operator to double
    inline operator double() const
    {
        return getDouble();
    }
    /// Inline cast operator to char*
    inline operator const char*() const
    {
        return getText();
    }
    /// Inline cast operator to void*
    inline operator const void*() const
    {
        return getBlob();
    }
#ifdef __GNUC__
    // NOTE : the following is required by GCC to cast a Column result in a std::string
    // (error: conversion from ‘SQLite::Column’ to non-scalar type ‘std::string {aka std::basic_string<char>}’ requested)
    // but is not working under Microsoft Visual Studio 2010 and 2012
    // (error C2440: 'initializing' : cannot convert from 'SQLite::Column' to 'std::basic_string<_Elem,_Traits,_Ax>'
    //  [...] constructor overload resolution was ambiguous)
    /// Inline cast operator to std::string
    inline operator const std::string() const
    {
        return getText();
    }
#endif

    /// Return UTF-8 encoded English language explanation of the most recent error.
    inline const char* errmsg(void) const
    {
        return sqlite3_errmsg(mStmtPtr);
    }
private:
    Statement::Ptr  mStmtPtr;   //!< Shared Pointer to the prepared SQLite Statement Object
    int             mIndex;     //!< Index of the column in the row of result
};

/**
 * @brief Standard std::ostream text inserter
 *
 * Insert the text value of the Column object, using getText(), into the provided stream.
 *
 * @param[in] aStream   Stream to use
 * @param[in] aColumn   Column object to insert into the provided stream
 *
 * @return  Reference to the stream used
 */
std::ostream& operator<<(std::ostream& aStream, const Column& aColumn);

}  // namespace SQLite