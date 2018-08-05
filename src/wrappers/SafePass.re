/* https://github.com/BuckleScript/bucklescript/issues/2952 */

type t;

[@bs.module "wrappers/SafePass"] external encode : 'a => t = "";
[@bs.module "wrappers/SafePass"] external decode : t => 'a = "";