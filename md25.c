#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>

static struct i2c_client *md25_client;

static ssize_t version_show(struct device *child,
		struct device_attribute *attr, char *buf)
{
	s32 ver = i2c_smbus_read_byte_data(md25_client, 0x13);

	return sprintf(buf, "%d\n", ver);
}

static ssize_t encoder_show(struct device *child,
		struct device_attribute *attr, char *buf)
{

	s32 enc1 = 0;
	s32 enc2 = 0;

	enc1  = i2c_smbus_read_byte_data(md25_client, 0x02) << 24;
	enc1 |= i2c_smbus_read_byte_data(md25_client, 0x03) << 16;
	enc1 |= i2c_smbus_read_byte_data(md25_client, 0x04) << 8;
	enc1 |= i2c_smbus_read_byte_data(md25_client, 0x05);

	enc2  = i2c_smbus_read_byte_data(md25_client, 0x06) << 24;
	enc2 |= i2c_smbus_read_byte_data(md25_client, 0x07) << 16;
	enc2 |= i2c_smbus_read_byte_data(md25_client, 0x08) << 8;
	enc2 |= i2c_smbus_read_byte_data(md25_client, 0x09);

	return sprintf(buf, "%d,%d\n", enc1, enc2);

}

static ssize_t speed_store(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	u8 s1, s2;

	if (sscanf(buf, "%hhu, %hhu", &s1, &s2) < 2)
		if (sscanf(buf, "%hhu,%hhu", &s1, &s2) < 2)
			return 0;

	i2c_smbus_write_byte_data(md25_client, 0x00, s1);
	i2c_smbus_write_byte_data(md25_client, 0x01, s2);
	return count;
}


static DEVICE_ATTR(version, 0444, version_show, NULL);
static DEVICE_ATTR(encoder, 0444, encoder_show, NULL);
static DEVICE_ATTR(speed, 0220, NULL, speed_store);

static struct attribute *md25_attrs[] = {
	&dev_attr_version.attr,
	&dev_attr_encoder.attr,
	&dev_attr_speed.attr,
	NULL,
};

static struct attribute_group md25_attr_group = {
	.attrs = md25_attrs,
};

static const struct attribute_group *md25_attr_groups[] = {
	&md25_attr_group,
	NULL,
};

static int md25_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{

	pr_debug("MD25 driver online\n");
	md25_client = client;
	return 0;
}

static int md25_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id md25_id[] = {
	{"md25", 0},
	{}
};
MODULE_DEVICE_TABLE(i2c, md25_id);

static struct i2c_driver md25_driver = {
	.probe		=	md25_probe,
	.remove		=	md25_remove,
	.id_table	=	md25_id,
	.driver = {
		.name	=	"md25",
		.owner = THIS_MODULE,
		.groups = md25_attr_groups,
	},
};


/*
 * To be able to compile this as an out-of-tree module,
 * the module init defines i2c board info and allocates the i2c adapter.
 * Normally this would be done in a board sepcific section under arch.
 * Now we just make this module Raspberry Pi 2 specific.
 */
static int __init md25_init(void)
{

	int ret;
	struct i2c_adapter *adapter;
	struct i2c_client *client;

	struct i2c_board_info info = {
		.type = "md25",
		.addr = 0x58,
	};

	ret = i2c_add_driver(&md25_driver);
	if (ret)
		return ret;

	adapter = i2c_get_adapter(1); /*on raspi2 the default i2c bus is i2c-1*/
	if (!adapter)
		return -EINVAL;

	client = i2c_new_device(adapter, &info);
	if (!client)
		return -EINVAL;

	pr_debug("MD25 init ok!\n");
	return 0;
}

static void __exit md25_exit(void)
{
	i2c_unregister_device(md25_client);
	i2c_del_driver(&md25_driver);
}

module_init(md25_init);
module_exit(md25_exit);

MODULE_AUTHOR("Mikko Seppälä <mikko.seppala@ppc.inet.fi>");
MODULE_DESCRIPTION("MD25 motor controller driver for Raspberry Pi 2");
MODULE_LICENSE("GPL");
