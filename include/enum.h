#ifndef ENUM
#define ENUM
#include <cassert>
#include <string>

enum class ContainerKind {
    None,
    Pan,
    Pot,
    Plate,
    DirtyPlates,
};

enum class TileKind {
    None,
    Void,
    Floor,
    Wall,
    Table,
    IngredientBox,
    Trashbin,
    Chop,
    ServiceWindow,
    Stove,
    PlateReturn,
    Sink,
    PlateRack,
};

inline TileKind getTileKind(char kindChar) {
    switch (kindChar) {
    case '_':
        return TileKind::Void;
    case '.':
        return TileKind::Floor;
    case '*':
        return TileKind::Table;
    case 't':
        return TileKind::Trashbin;
    case 'c':
        return TileKind::Chop;
    case '$':
        return TileKind::ServiceWindow;
    case 's':
        return TileKind::Stove;
    case 'p':
        return TileKind::PlateReturn;
    case 'k':
        return TileKind::Sink;
    case 'r':
        return TileKind::PlateRack;
    default:
        break;
        //assert(0);
    }
}

inline char getAbbrev(TileKind kind) {
    switch (kind) {
    case TileKind::IngredientBox:
        return 'i';
    case TileKind::Trashbin:
        return 't';
    case TileKind::Chop:
        return 'c';
    case TileKind::ServiceWindow:
        return '$';
    case TileKind::Stove:
        return 's';
    case TileKind::PlateReturn:
        return 'p';
    case TileKind::Sink:
        return 'k';
    case TileKind::PlateRack:
        return 'r';
    default:
        break;
        //assert(0);
    }
}

enum Actions{
    Move,
    Interact,
    Put,
    Pick,
};

enum Status{
    free_,
    busy,
};

#endif