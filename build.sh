#!/bin/bash

# Script de build pour HiddenInk
# Usage: ./build.sh [clean|release|debug|run]

set -e  # Arrêter en cas d'erreur

BUILD_DIR="build"
BIN_NAME="hiddenink"

# Couleurs pour l'affichage
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Fonction d'affichage
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Fonction de nettoyage
clean() {
    print_info "Nettoyage du projet..."
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
        print_success "Dossier build supprimé"
    else
        print_warning "Aucun dossier build à supprimer"
    fi
}

# Fonction de build
build() {
    local BUILD_TYPE=${1:-Debug}
    
    print_info "Configuration du projet (mode $BUILD_TYPE)..."
    
    # Créer le dossier build s'il n'existe pas
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # Configurer avec CMake
    cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..
    
    # Compiler
    print_info "Compilation en cours..."
    cmake --build . --config $BUILD_TYPE
    
    cd ..
    
    if [ -f "$BUILD_DIR/bin/$BIN_NAME" ] || [ -f "$BUILD_DIR/bin/${BIN_NAME}.exe" ]; then
        print_success "Compilation réussie !"
        print_info "Exécutable : $BUILD_DIR/bin/$BIN_NAME"
    else
        print_error "Échec de la compilation"
        exit 1
    fi
}

# Fonction pour exécuter le programme
run() {
    if [ -f "$BUILD_DIR/bin/$BIN_NAME" ]; then
        print_info "Lancement de HiddenInk..."
        "$BUILD_DIR/bin/$BIN_NAME" "$@"
    elif [ -f "$BUILD_DIR/bin/${BIN_NAME}.exe" ]; then
        print_info "Lancement de HiddenInk..."
        "$BUILD_DIR/bin/${BIN_NAME}.exe" "$@"
    else
        print_error "Exécutable non trouvé. Veuillez compiler d'abord."
        exit 1
    fi
}

# Fonction d'aide
show_help() {
    echo "Usage: ./build.sh [OPTION]"
    echo ""
    echo "Options:"
    echo "  clean      Nettoyer le projet (supprimer build/)"
    echo "  debug      Compiler en mode Debug (par défaut)"
    echo "  release    Compiler en mode Release (optimisé)"
    echo "  run        Exécuter le programme (compile si nécessaire)"
    echo "  help       Afficher cette aide"
    echo ""
    echo "Exemples:"
    echo "  ./build.sh              # Compile en mode Debug"
    echo "  ./build.sh release      # Compile en mode Release"
    echo "  ./build.sh clean        # Nettoie le projet"
    echo "  ./build.sh run          # Compile et exécute"
}

# Traitement des arguments
case "${1:-debug}" in
    clean)
        clean
        ;;
    debug)
        build Debug
        ;;
    release)
        build Release
        ;;
    run)
        if [ ! -f "$BUILD_DIR/bin/$BIN_NAME" ] && [ ! -f "$BUILD_DIR/bin/${BIN_NAME}.exe" ]; then
            build Debug
        fi
        shift  # Enlever 'run' des arguments
        run "$@"
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        print_error "Option inconnue: $1"
        show_help
        exit 1
        ;;
esac

print_success "Terminé !"
