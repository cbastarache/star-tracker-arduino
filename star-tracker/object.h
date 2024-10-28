#ifndef __OBJECT__H__
#define __OBJECT__H__

class Object{
public:

    enum Type {
        STAR,
        PLANET,
        SATTELITE
    };

    Object(String n_name, Type n_type){
        name = n_name;
        type = n_type;
    }

    void getLocation() {}
    void getPath() {}
    String getName() { return name; }
    Type getType() { return type; }
    

    Type type;
    String name;
private:

};

#endif