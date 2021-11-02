@0xf7e8a7a46c403523;

struct RPCSettingsParam {
    id              @0 : Int16;
    key             @1 : Text;
    title           @2 : Text;
    value           @3 : List(Text);
    type            @4 : Type;
    enumValues      @5 : List(Text);
    attributes      @6 : Int16;
    description     @7 : Text;

    enum Type {
        none           @0;
        bool           @1;
        numeric        @2;
        text           @3;
        listNumeric    @4;
        listText       @5;
        enum           @6;
        file           @7;
        listFile       @8;
        directory      @9;
        listDirectory  @10;
        bardcode2d     @11;
        bardcode3d     @12;
    }
}

enum Attribute {
        none          @0;
        required      @1;
        protected     @2;
        dummy3        @3;
        readonly      @4;
        dummy5        @5;
        dummy6        @6;
        dummy7        @7;
        list          @8;
        dummy8        @9;
        dummy9        @10;
        dummy10       @11;
        dummy11       @12;
        dummy12       @13;
        dummy13       @14;
        dummy14       @15;
        hidden        @16;
        }

struct RPCSettingsSection{
    id          @0 : Int16;
    name        @1 : Text;
    title       @2 : Text;
    icon        @3 : Text;
    attributes  @4 : Int32;
    description @5 : Text;
    subsections @6 : List(RPCSettingsSection);
    params      @7 : List(RPCSettingsParam);
}

struct RPCSettingsModule {
    title       @0 : Text;
    description @1 : Text;
    icon        @2 : Text;
}

struct RPCSettingsResult {
    result      @0 : Bool;
    errors      @1 : List(Text);
}

interface RPCSettingsService {
    getModulesList    @0  ()                                              -> (modules       : List(RPCSettingsModule));
    getModuleSections @1  (module  : Text)                                -> (sections      : List(RPCSettingsSection));
    getSection        @2  (module  : Text, section  : Text)                                                  -> (sections      : List(RPCSettingsSection));
    addSection        @3  (module  : Text, section  : Text, sectionValue : RPCSettingsSection)               -> (result        : RPCSettingsResult);
    removeSection     @4  (module  : Text, section  : Text, id     : Int16)                                  -> (result        : RPCSettingsResult);
    editSection       @5  (module  : Text, section  : Text, id     : Int16, sectionValue :RPCSettingsSection)-> (result        : RPCSettingsResult);
    apply             @6  (module  : Text)                                                                   -> (result        : Int16);
}
