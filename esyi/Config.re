type t = {
  esySolveCmd: Cmd.t,
  sourceArchivePath: Path.t,
  sourceStagePath: Path.t,
  sourceInstallPath: Path.t,
  opamArchivesIndexPath: Path.t,
  esyOpamOverride: checkout,
  opamRepository: checkout,
  npmRegistry: string,
  solveTimeout: float,
  skipRepositoryUpdate: bool,
}
and checkout =
  | Local(Path.t)
  | Remote(string, Path.t)
and checkoutCfg = [
  | `Local(Path.t)
  | `Remote(string)
  | `RemoteLocal(string, Path.t)
];

let resolvedPrefix = "esyi5-";

let esyOpamOverrideVersion = "6";

let configureCheckout = (~defaultRemote, ~defaultLocal) =>
  fun
  | Some(`RemoteLocal(remote, local)) => Remote(remote, local)
  | Some(`Remote(remote)) => Remote(remote, defaultLocal)
  | Some(`Local(local)) => Local(local)
  | None => Remote(defaultRemote, defaultLocal);

let make =
    (
      ~npmRegistry=?,
      ~cachePath=?,
      ~cacheTarballsPath=?,
      ~cacheSourcesPath=?,
      ~opamRepository=?,
      ~esyOpamOverride=?,
      ~solveTimeout=60.0,
      ~esySolveCmd,
      ~skipRepositoryUpdate,
      (),
    ) => {
  open RunAsync.Syntax;
  let%bind cachePath =
    RunAsync.ofRun(
      Run.Syntax.(
        switch (cachePath) {
        | Some(cachePath) => return(cachePath)
        | None =>
          let userDir = Path.homePath();
          return(Path.(userDir / ".esy"));
        }
      ),
    );

  let sourcePath =
    switch (cacheSourcesPath) {
    | Some(path) => path
    | None => Path.(cachePath / "source")
    };
  let%bind () = Fs.createDir(sourcePath);

  let sourceArchivePath =
    switch (cacheTarballsPath) {
    | Some(path) => path
    | None => Path.(sourcePath / "a")
    };
  let%bind () = Fs.createDir(sourceArchivePath);

  let sourceStagePath = Path.(sourcePath / "s");
  let%bind () = Fs.createDir(sourceStagePath);

  let sourceInstallPath = Path.(sourcePath / "i");
  let%bind () = Fs.createDir(sourceInstallPath);

  let opamArchivesIndexPath = Path.(cachePath / "opam-urls.txt");

  let opamRepository = {
    let defaultRemote = "https://github.com/ocaml/opam-repository";
    let defaultLocal = Path.(cachePath / "opam-repository");
    configureCheckout(~defaultLocal, ~defaultRemote, opamRepository);
  };

  let esyOpamOverride = {
    let defaultRemote = "https://github.com/esy-ocaml/esy-opam-override";
    let defaultLocal = Path.(cachePath / "esy-opam-override");
    configureCheckout(~defaultLocal, ~defaultRemote, esyOpamOverride);
  };

  let npmRegistry =
    Option.orDefault(~default="http://registry.npmjs.org/", npmRegistry);

  return({
    esySolveCmd,
    sourceArchivePath,
    sourceStagePath,
    sourceInstallPath,
    opamArchivesIndexPath,
    opamRepository,
    esyOpamOverride,
    npmRegistry,
    skipRepositoryUpdate,
    solveTimeout,
  });
};
