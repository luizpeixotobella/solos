// SPDX-License-Identifier: MIT
pragma solidity ^0.8.28;

import {ERC1155} from "@openzeppelin/contracts/token/ERC1155/ERC1155.sol";
import {Ownable} from "@openzeppelin/contracts/access/Ownable.sol";

/// @notice LOCAL PROTOTYPE ONLY. Not the existing Polygon artwork contract.
/// @dev Payment settlement is an issuer attestation, not something this contract proves.
contract FounderHeartPass is ERC1155, Ownable {
    uint256 public constant PATRONO_PASS = 1;
    enum Status { Missing, Approved, Claimed, Revoked }
    struct Entitlement { address wallet; Status status; }
    mapping(bytes32 => Entitlement) public entitlements;
    mapping(address => uint256) public activeClaims;

    event SupportApproved(bytes32 indexed supportRef, address indexed wallet);
    event SupportClaimed(bytes32 indexed supportRef, address indexed wallet);
    event SupportRevoked(bytes32 indexed supportRef, address indexed wallet);

    constructor(address issuer) ERC1155("") Ownable(issuer) {}

    function approveSupport(bytes32 supportRef, address wallet) external onlyOwner {
        require(wallet != address(0), "missing_wallet");
        require(supportRef != bytes32(0), "missing_supportRef");
        require(entitlements[supportRef].status == Status.Missing, "supportRef_already_used");
        entitlements[supportRef] = Entitlement(wallet, Status.Approved);
        emit SupportApproved(supportRef, wallet);
    }

    function claim(bytes32 supportRef) external {
        Entitlement storage e = entitlements[supportRef];
        require(e.status == Status.Approved, "claim_not_available");
        require(e.wallet == msg.sender, "wrong_wallet");
        e.status = Status.Claimed;
        activeClaims[msg.sender] += 1;
        // Multiple supports never create multiple personal access passes.
        if (balanceOf(msg.sender, PATRONO_PASS) == 0) _mint(msg.sender, PATRONO_PASS, 1, "");
        emit SupportClaimed(supportRef, msg.sender);
    }

    function revokeSupport(bytes32 supportRef) external onlyOwner {
        Entitlement storage e = entitlements[supportRef];
        require(e.status == Status.Approved || e.status == Status.Claimed, "nothing_to_revoke");
        if (e.status == Status.Claimed) {
            activeClaims[e.wallet] -= 1;
            if (activeClaims[e.wallet] == 0) _burn(e.wallet, PATRONO_PASS, 1);
        }
        e.status = Status.Revoked;
        emit SupportRevoked(supportRef, e.wallet);
    }

    // This proposal makes access personal; it does NOT change the artistic NFT.
    function _update(address from, address to, uint256[] memory ids, uint256[] memory values)
        internal override
    {
        require(from == address(0) || to == address(0), "personal_pass_not_transferable");
        super._update(from, to, ids, values);
    }
}
