# public key 1: 0xb8e4faf9bbf2e536f2b34e83577991df54a2234764a02434c9d03bf65c212666
# private key 1: 0x8292e116b8609a7ccc3599363bc60f5f9e3646c2fce1e062490875dbae1b624eb8e4faf9bbf2e536f2b34e83577991df54a2234764a02434c9d03bf65c212666

# public key 2: 0x93463e3fee4c0ad27a672c34ecd015e9eb09ae61dbe075ba79ed2425ef029cc7
# private key 2: 0x32605c6eaaf3a62cbc72a9cb09be8e86aa859fd46f3924a0f97002fbe26e3e3093463e3fee4c0ad27a672c34ecd015e9eb09ae61dbe075ba79ed2425ef029cc7

curl -X POST -H "Content-Type: application/json" http://127.0.0.1:2826/transaction --data-raw '{
  "inputs": [],
  "outputs": [
    {
      "amount": 50000000,
      "key": "0xb8e4faf9bbf2e536f2b34e83577991df54a2234764a02434c9d03bf65c212666"
    },
    {
      "amount": 50000000,
      "key": "0x93463e3fee4c0ad27a672c34ecd015e9eb09ae61dbe075ba79ed2425ef029cc7"
    }
  ]
}'

curl -X POST -H "Content-Type: application/json" http://127.0.0.1:2826/transaction --data-raw '{
  "inputs": [
    {
      "hash": "0xcb549345060bf65c097ef2d3628221904ead600a0560ecc3066146c1d1c5da28",
      "index": 0,
      "sig": "0x6ec902bb7c814c3f7714659a9d16a4566f4e3ded05ce71bbaae08c26fff32c9032de93e35a3362a26a7d3cfaa32d076514b5eccb8fe2170433d18d04decb4702"
    },
    {
      "hash": "0xcb549345060bf65c097ef2d3628221904ead600a0560ecc3066146c1d1c5da28",
      "index": 1,
      "sig": "0x2b4a27a99cec34aa3778e34db7ff4daef87c2d7006c68d3553f57ca4b416356708c5463f6ec489456fd15198a0ae6717976c3737f51d6d63976b0e69f2074b05"
    }
  ],
  "outputs": [
    {
      "amount": 4000000,
      "key": "0xb8e4faf9bbf2e536f2b34e83577991df54a2234764a02434c9d03bf65c212666"
    },
    {
      "amount": 96000000,
      "key": "0x93463e3fee4c0ad27a672c34ecd015e9eb09ae61dbe075ba79ed2425ef029cc7"
    }
  ]
}'
