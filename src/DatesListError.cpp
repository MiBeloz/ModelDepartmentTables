#include "DatesListError.h"

DatesListError::DatesListError(ErrorType error) : m_error(error) { }

DatesListError::ErrorType DatesListError::lastError() const{
    return m_error;
}
