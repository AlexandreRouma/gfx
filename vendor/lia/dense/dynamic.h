#pragma once
#include "../force_inline.h"

namespace lia {
    /**
     * Dynamically allocated dense matrix.
    */
    template <typename DT>
    class DMat {
    public:
        // Default constructor
        DMat();

        /**
         * Create a matrix.
         * @param lines Number of lines.
         * @param columns Number of columns.
        */
        DMat(int lines, int columns);

        // Copy constructor
        DMat(const DMat& copy);

        // Move constructor
        DMat(DMat&& move);

        // Destructor
        ~DMat();

        // Function operator to access elements
        constexpr LIA_FORCE_INLINE DT& operator()(int line, int column = 0) { return _data[line*cs + column]; }
        constexpr LIA_FORCE_INLINE const DT& operator()(int line, int column = 0) const { return _data[line*cs + column]; }

        // Array operator to access the data
        constexpr LIA_FORCE_INLINE DT& operator[](int id) { return _data[id]; }
        constexpr LIA_FORCE_INLINE const DT& operator[](int id) const { return _data[id]; }

        /**
         * Get the raw data buffer from the matrix.
         * @return Raw data buffer containing the matrix data.
        */
        constexpr LIA_FORCE_INLINE DT* data() { return _data; }
        constexpr LIA_FORCE_INLINE const DT* data() const { return _data; }

        // Number of lines
        const int ls;

        // Number of columns
        const int cs;

    private:
        // Raw matrix data
        DT* _data;
    };

    /**
     * Dynamically allocated vector. Equivalent to a statically allocated matrix with a single column.
    */
    template <typename DT>
    class DVec : public DMat<DT> {
    public:
        // Default constructor
        DVec();
        
        /**
         * Create a vector.
         * @param lines Number of lines.
        */
        DVec(int lines);
    };

    // Common dynamic vector and matrix types
    using DVecd = DVec<double>;
    using DVecf = DVec<float>;
    using DVeci = DVec<int>;
    using DMatd = DMat<double>;
    using DMatf = DMat<float>;
    using DMati = DMat<int>;

    // ================================= CAST =================================

    /**
     * Cast a matrix or vector to another type.
     * @param result Matrix or vector of the destination type.
     * @param value Matrix or vector of the source type.
    */
    template <typename TA, typename TB>
    void cast(DVec<TB>& result, const DVec<TA>& value);
    template <typename TA, typename TB>
    void cast(DMat<TB>& result, const DMat<TA>& value);

    // ================================= CLEAR =================================

    /**
     * Clear a matrix or vector with a specific value (zero by default).
     * @param result Matrix or vector to clear.
     * @param value Value to clear the matrix or vector with.
    */
    template <typename T>
    void clear(DVec<T>& result, T value = 0.0);
    template <typename T>
    void clear(DMat<T>& result, T value = 0.0);

    // =============================== TRANSPOSE ===============================

    /**
     * Transpose a matrix or vector.
     * @param result Matrix or vector to write the result to.
     * @param value Matrix or vector to transpose.
    */
    template <typename T>
    void transpose(DMat<T>& result, const DVec<T>& value);
    template <typename T>
    void transpose(DVec<T>& result, const DMat<T>& value);
    template <typename T>
    void transpose(DMat<T>& result, const DMat<T>& value);

    // ================================= NORM =================================

    /**
     * Compute the euclidian norm of a vector.
     * @param value Vector to take the euclidian norm of.
     * @return Euclidian norm of the vector.
    */
    template <typename T>
    T norm(const DVec<T>& value);

    // =============================== ADDITION ===============================

    /**
     * Add two matrices or vectors.
     * @param result Matrix or vector to write the result to.
     * @param left Left-hand matrix or vector.
     * @param right Right-hand matrix or vector.
    */
    template <typename T>
    void add(DVec<T>& result, const DVec<T>& left, const DVec<T>& right);
    template <typename T>
    void add(DMat<T>& result, const DMat<T>& left, const DMat<T>& right);

    // ============================== SUBTRACTION ==============================

    /**
     * Subtract two matrices or vectors.
     * @param result Matrix or vector to write the result to.
     * @param left Left-hand matrix or vector.
     * @param right Right-hand matrix or vector.
    */
    template <typename T>
    void sub(DVec<T>& result, const DVec<T>& left, const DVec<T>& right);
    template <typename T>
    void sub(DMat<T>& result, const DMat<T>& left, const DMat<T>& right);

    // ============================ MULTIPLICATION ============================

    /**
     * Do scalar multiplication between a matrix or vector and a scalar.
     * @param result Matrix, vector or scalar to write the result to.
     * @param left Matrix or vector to multiply.
     * @param right Scalar to multiply the matrix or vector by.
    */
    template <typename T>
    void mul(DVec<T>& result, const DVec<T>& left, T right);
    template <typename T>
    void mul(DMat<T>& result, const DMat<T>& left, T right);

    // =============================== DIVISION ===============================

    /**
     * Do scalar divison between a matrix or vector and a scalar.
     * @param result Matrix, vector or scalar to write the result to.
     * @param left Matrix or vector to divide.
     * @param right Scalar to divide the matrix or vector by.
    */
    template <typename T>
    void div(DVec<T>& result, const DVec<T>& left, T right);
    template <typename T>
    void div(DMat<T>& result, const DMat<T>& left, T right);

    // ============================== DOT PRODUCT ==============================

    /**
     * Take the dot product between two vector or matrices.
     * @param result Matrix, vector or scalar to write the result to.
     * @param left Left-hand matrix or vector.
     * @param right Right-hand matrix or vector.
    */
    template <typename T>
    void dot(T& result, const DVec<T>& left, const DVec<T>& right);
    template <typename T>
    void dot(DVec<T>& result, const DMat<T>& left, const DVec<T>& right);
    template <typename T>
    void dot(DMat<T>& result, const DMat<T>& left, const DMat<T>& right);

    // ============================= CROSS PRODUCT =============================

    /**
     * Take the cross product between two 3D vectors.
     * @param result Vector to write the result to.
     * @param left Left-hand side vector.
     * @param right Right-hand side vector.
    */
    template <typename T>
    void cross(DVec<T>& result, const DVec<T>& left, const DVec<T>& right);
}