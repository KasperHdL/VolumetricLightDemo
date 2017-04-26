#Code Style

Prefer the Header files to Source files, limit as much as possible. if something has to move to a source, all function bodies has to go.

The following code style: 

class HelloWorld{
    private:
        Type* _type_yep; //@Note(Kasper) * is on the Type because it is  a pointer type!
        void _hello_world(){

        }

    public:
        Type* type_yup;
        void hello_world(){

        }
};
