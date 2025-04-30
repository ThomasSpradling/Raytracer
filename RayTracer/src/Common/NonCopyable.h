#pragma once

/**
 * @class NonCopyable
 * @brief Interface to be implemented by classes that should never be copied.
 * 
 * This is a utility interface to be implemented by other classes whose copy constructor and
 * assignment operator should be deleted.
 */
class NonCopyable {
public:

    /** @brief Copy constructor deleted. */
    NonCopyable(const NonCopyable &other) = delete;

    /** @brief Copy assignment operator deleted. */
    NonCopyable &operator=(const NonCopyable &other) = delete;

protected:
    /** @brief Private default constructor. */
    NonCopyable() = default;

    /** @brief Private default destructor. */
    ~NonCopyable() = default;
};
