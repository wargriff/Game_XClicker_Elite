"""Unified bootstrap — one load sequence for the iCUE interface."""

from dataclasses import dataclass
from typing import Callable, Optional

from core.engine import MacroManager
from services.engine_proxy import EngineProxy
from services.node_bridge import NodeBridge
from services.profile_manager import ProfileManager
from services.sidecar_api import SidecarAPI
from utils.debug import log

ProgressCallback = Callable[[int, str], None]


@dataclass
class BootContext:
    manager: MacroManager
    proxy: EngineProxy
    profiles: ProfileManager
    sidecar: SidecarAPI
    node: NodeBridge


def _report(cb: Optional[ProgressCallback], percent: int, message: str):
    if cb:
        cb(percent, message)


def bootstrap(progress: Optional[ProgressCallback] = None) -> BootContext:
    """Initialize engine, profiles, sidecar and Node.js in one sequence."""
    log("BOOT", "démarrage bootstrap")

    _report(progress, 5, "Initialisation du moteur Win32…")
    manager = MacroManager()
    log("BOOT", "moteur OK")

    _report(progress, 25, "Connexion des macros souris / clavier…")
    proxy = EngineProxy(manager)
    log("BOOT", "proxy OK")

    _report(progress, 45, "Chargement du profil…")
    profiles = ProfileManager()
    profiles.load("default")
    profiles.apply_to_engine(manager)
    log("BOOT", "profil OK")

    _report(progress, 60, "Démarrage API Sidecar (port 17840)…")
    sidecar = SidecarAPI(proxy, profiles=profiles)
    sidecar.start()
    log("BOOT", f"sidecar online={sidecar.online}")

    _report(progress, 75, "Démarrage Node.js (port 5173)…")
    node = NodeBridge()
    node.start()
    log("BOOT", f"node online={node.online}")

    _report(progress, 90, "Préparation interface iCUE…")

    return BootContext(
        manager=manager,
        proxy=proxy,
        profiles=profiles,
        sidecar=sidecar,
        node=node,
    )
