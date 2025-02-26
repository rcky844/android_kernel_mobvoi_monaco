// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2014-2020, The Linux Foundation. All rights reserved.
 */

#include <linux/dma-mapping.h>
#include <linux/of_address.h>

#include "cam_compat.h"
#include "cam_debug_util.h"


#if KERNEL_VERSION(5, 4, 0) <= LINUX_VERSION_CODE
int cam_reserve_icp_fw(struct cam_fw_alloc_info *icp_fw, size_t fw_length)
{
	int rc = 0;
	struct device_node *of_node;
	struct device_node *mem_node;
	struct resource     res;

	of_node = (icp_fw->fw_dev)->of_node;
	mem_node = of_parse_phandle(of_node, "memory-region", 0);
	if (!mem_node) {
		rc = -ENOMEM;
		CAM_ERR(CAM_SMMU, "FW memory carveout not found");
		goto end;
	}
	rc = of_address_to_resource(mem_node, 0, &res);
	of_node_put(mem_node);
	if (rc < 0) {
		CAM_ERR(CAM_SMMU, "Unable to get start of FW mem carveout");
		goto end;
	}
	icp_fw->fw_hdl = res.start;
	icp_fw->fw_kva = ioremap_wc(icp_fw->fw_hdl, fw_length);
	if (!icp_fw->fw_kva) {
		CAM_ERR(CAM_SMMU, "Failed to map the FW.");
		rc = -ENOMEM;
		goto end;
	}

	memset_io(icp_fw->fw_kva, 0, fw_length);

end:
	return rc;
}

void cam_unreserve_icp_fw(struct cam_fw_alloc_info *icp_fw, size_t fw_length)
{
	iounmap(icp_fw->fw_kva);
}

/*
int cam_ife_notify_safe_lut_scm(bool safe_trigger)
{
	const uint32_t smmu_se_ife = 0;
	uint32_t camera_hw_version, rc = 0;

	rc = cam_cpas_get_cpas_hw_version(&camera_hw_version);
	if (!rc && qcom_scm_smmu_notify_secure_lut(smmu_se_ife, safe_trigger)) {
		switch (camera_hw_version) {
		case CAM_CPAS_TITAN_170_V100:
		case CAM_CPAS_TITAN_170_V110:
		case CAM_CPAS_TITAN_175_V100:
			CAM_ERR(CAM_ISP, "scm call to enable safe failed");
			rc = -EINVAL;
			break;
		default:
			break;
		}
	}

	return rc;
}

int cam_csiphy_notify_secure_mode(struct csiphy_device *csiphy_dev,
	bool protect, int32_t offset)
{
	int rc = 0;

	if (offset >= CSIPHY_MAX_INSTANCES) {
		CAM_ERR(CAM_CSIPHY, "Invalid CSIPHY offset");
		rc = -EINVAL;
	} else if (qcom_scm_camera_protect_phy_lanes(protect,
			csiphy_dev->csiphy_cpas_cp_reg_mask[offset])) {
		CAM_ERR(CAM_CSIPHY, "SCM call to hypervisor failed");
		rc = -EINVAL;
	}

	return 0;
}

void cam_cpastop_scm_write(struct cam_cpas_hw_errata_wa *errata_wa)
{
	int reg_val;

	qcom_scm_io_readl(errata_wa->data.reg_info.offset, &reg_val);
	reg_val |= errata_wa->data.reg_info.value;
	qcom_scm_io_writel(errata_wa->data.reg_info.offset, reg_val);
}

static int camera_platform_compare_dev(struct device *dev, const void *data)
{
	return platform_bus_type.match(dev, (struct device_driver *) data);
}*/
#else
int cam_reserve_icp_fw(struct cam_fw_alloc_info *icp_fw, size_t fw_length)
{
	int rc = 0;

	icp_fw->fw_kva = dma_alloc_coherent(icp_fw->fw_dev, fw_length,
		&icp_fw->fw_hdl, GFP_KERNEL);

	if (!icp_fw->fw_kva) {
		CAM_ERR(CAM_SMMU, "FW memory alloc failed");
		rc = -ENOMEM;
	}

	return rc;
}

void cam_unreserve_icp_fw(struct cam_fw_alloc_info *icp_fw, size_t fw_length)
{
	dma_free_coherent(icp_fw->fw_dev, fw_length, icp_fw->fw_kva,
		icp_fw->fw_hdl);
}

int cam_ife_notify_safe_lut_scm(bool safe_trigger)
{
	const uint32_t smmu_se_ife = 0;
	uint32_t camera_hw_version, rc = 0;
	struct scm_desc description = {
		.arginfo = SCM_ARGS(2, SCM_VAL, SCM_VAL),
		.args[0] = smmu_se_ife,
		.args[1] = safe_trigger,
	};

	rc = cam_cpas_get_cpas_hw_version(&camera_hw_version);
	if (!rc && scm_call2(SCM_SIP_FNID(0x15, 0x3), &description)) {
		switch (camera_hw_version) {
		case CAM_CPAS_TITAN_170_V100:
		case CAM_CPAS_TITAN_170_V110:
		case CAM_CPAS_TITAN_175_V100:
			CAM_ERR(CAM_ISP, "scm call to enable safe failed");
			rc = -EINVAL;
			break;
		default:
			break;
		}
	}

	return rc;
}

int cam_csiphy_notify_secure_mode(struct csiphy_device *csiphy_dev,
	bool protect, int32_t offset)
{
	int rc = 0;
	struct scm_desc description = {
		.arginfo = SCM_ARGS(2, SCM_VAL, SCM_VAL),
		.args[0] = protect,
		.args[1] = csiphy_dev->csiphy_cpas_cp_reg_mask[offset],
	};

	if (offset >= CSIPHY_MAX_INSTANCES) {
		CAM_ERR(CAM_CSIPHY, "Invalid CSIPHY offset");
		rc = -EINVAL;
	} else if (scm_call2(SCM_SIP_FNID(0x18, 0x7), &description)) {
		CAM_ERR(CAM_CSIPHY, "SCM call to hypervisor failed");
		rc = -EINVAL;
	}

	return 0;
}

void cam_cpastop_scm_write(struct cam_cpas_hw_errata_wa *errata_wa)
{
	int reg_val;

	reg_val = scm_io_read(errata_wa->data.reg_info.offset);
	reg_val |= errata_wa->data.reg_info.value;
	scm_io_write(errata_wa->data.reg_info.offset, reg_val);
}

static int camera_platform_compare_dev(struct device *dev, void *data)
{
	return platform_bus_type.match(dev, (struct device_driver *) data);
}
#endif


