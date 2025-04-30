#pragma once

/**
 * @class NonMovable
 * @brief Interface to be implemented by classes that should never be moved.
 * 
 * This is a utility interface to be implemented by other classes whose move constructor and
 * assignment operator should be deleted.
 */
class NonMovable {
public:

    /** @brief Move constructor deleted. */
    NonMovable(NonMovable &&other) = delete;

    /** @brief Move assignment operator deleted. */
    NonMovable &operator=(NonMovable &&other) = delete;

protected:
    /** @brief Private default constructor. */
    NonMovable() = default;

    /** @brief Private default destructor. */
    ~NonMovable() = default;
};
