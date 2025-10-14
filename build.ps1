# Script de build pour HiddenInk (Windows PowerShell)
# Usage: .\build.ps1 [clean|release|debug|run]

param(
    [Parameter(Position=0)]
    [ValidateSet("clean", "debug", "release", "run", "help")]
    [string]$Action = "debug",
    
    [Parameter(ValueFromRemainingArguments=$true)]
    [string[]]$Args
)

$BuildDir = "build"
$BinName = "hiddenink.exe"

# Fonctions d'affichage avec couleurs
function Print-Info {
    param([string]$Message)
    Write-Host "[INFO] " -ForegroundColor Blue -NoNewline
    Write-Host $Message
}

function Print-Success {
    param([string]$Message)
    Write-Host "[SUCCESS] " -ForegroundColor Green -NoNewline
    Write-Host $Message
}

function Print-Warning {
    param([string]$Message)
    Write-Host "[WARNING] " -ForegroundColor Yellow -NoNewline
    Write-Host $Message
}

function Print-Error {
    param([string]$Message)
    Write-Host "[ERROR] " -ForegroundColor Red -NoNewline
    Write-Host $Message
}

# Fonction de nettoyage
function Clean-Project {
    Print-Info "Nettoyage du projet..."
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
        Print-Success "Dossier build supprimé"
    } else {
        Print-Warning "Aucun dossier build à supprimer"
    }
}

# Fonction de build
function Build-Project {
    param([string]$BuildType = "Debug")
    
    Print-Info "Configuration du projet (mode $BuildType)..."
    
    # Créer le dossier build s'il n'existe pas
    if (-not (Test-Path $BuildDir)) {
        New-Item -ItemType Directory -Path $BuildDir | Out-Null
    }
    
    Push-Location $BuildDir
    
    try {
        # Configurer avec CMake
        cmake -DCMAKE_BUILD_TYPE=$BuildType ..
        
        if ($LASTEXITCODE -ne 0) {
            throw "Échec de la configuration CMake"
        }
        
        # Compiler
        Print-Info "Compilation en cours..."
        cmake --build . --config $BuildType
        
        if ($LASTEXITCODE -ne 0) {
            throw "Échec de la compilation"
        }
        
        Pop-Location
        
        $ExePath = Join-Path $BuildDir "bin" $BinName
        if (Test-Path $ExePath) {
            Print-Success "Compilation réussie !"
            Print-Info "Exécutable : $ExePath"
        } else {
            throw "Exécutable non trouvé après la compilation"
        }
    }
    catch {
        Pop-Location
        Print-Error $_.Exception.Message
        exit 1
    }
}

# Fonction pour exécuter le programme
function Run-Program {
    param([string[]]$Arguments)
    
    $ExePath = Join-Path $BuildDir "bin" $BinName
    
    if (Test-Path $ExePath) {
        Print-Info "Lancement de HiddenInk..."
        & $ExePath @Arguments
    } else {
        Print-Error "Exécutable non trouvé. Veuillez compiler d'abord."
        exit 1
    }
}

# Fonction d'aide
function Show-Help {
    Write-Host @"
Usage: .\build.ps1 [OPTION]

Options:
  clean      Nettoyer le projet (supprimer build/)
  debug      Compiler en mode Debug (par défaut)
  release    Compiler en mode Release (optimisé)
  run        Exécuter le programme (compile si nécessaire)
  help       Afficher cette aide

Exemples:
  .\build.ps1              # Compile en mode Debug
  .\build.ps1 release      # Compile en mode Release
  .\build.ps1 clean        # Nettoie le projet
  .\build.ps1 run          # Compile et exécute
  .\build.ps1 run hide-text image.png "message" output.png
"@
}

# Traitement des actions
switch ($Action) {
    "clean" {
        Clean-Project
    }
    "debug" {
        Build-Project -BuildType "Debug"
    }
    "release" {
        Build-Project -BuildType "Release"
    }
    "run" {
        $ExePath = Join-Path $BuildDir "bin" $BinName
        if (-not (Test-Path $ExePath)) {
            Build-Project -BuildType "Debug"
        }
        Run-Program -Arguments $Args
    }
    "help" {
        Show-Help
        exit 0
    }
    default {
        Print-Error "Option inconnue: $Action"
        Show-Help
        exit 1
    }
}

Print-Success "Terminé !"
