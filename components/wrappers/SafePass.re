/* https://github.com/BuckleScript/bucklescript/issues/2952 */

type t;

[@bs.module "./SafePass"] external encode : 'a => t = "";
[@bs.module "./SafePass"] external decode : t => 'a = "";