add_executable(ModelDepartmentTables main.cpp)

target_link_libraries(ModelDepartmentTables
    PRIVATE
        ModelDepartmentTables_lib  # Линкуем с библиотекой
        Qt6::Core
        Qt6::Widgets  # если используете виджеты
)