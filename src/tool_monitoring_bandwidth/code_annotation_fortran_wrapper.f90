module cfuncs
    use iso_c_binding, only : C_CHAR, C_NULL_CHAR
    interface
        subroutine yamb_annotate_set_event(string, ccc) bind(C, name = "yamb_annotate_set_event")
            use iso_c_binding, only : c_char, c_char
            character(kind = c_char) :: string(*)
            character(kind = c_char) :: ccc(*)
        end subroutine yamb_annotate_set_event
    end interface
end module



!Exemple
program mkdoor
    use cfuncs
    character (len = 128)color, event
    character (len = 32)subsub, sub

    color = "rouge"//C_NULL_CHAR
    event = "Ping"//C_NULL_CHAR

    call yamb_annotate_set_event("magenta\\C_NULL_CHAR", color )

end program