#<cldoc:fort::myrmidon::priv>

Private namespace for implementation

# Private namespace for implementation details

The myrmdion library aims to provide ABI stability using the private
implementation design pattern: Any public C++ class is only allowed to
have a pointer to a private implementation class.

All these private implementation are in the priv namespace. User of
the myrmidon API does not have to know these implementation details.

Any changes in these namespace can break ABI compatibility if the ABI
of exported classes in <myrmidon> are left unchanged.
