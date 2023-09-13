from bitcoinutils.script import Script



#[PUT YOUR BITCOIN SCRIPT HERE to see the encode]
scriptHEX='51b27576a914508bc12259f07e979e752ffc315981a7c9d326e4876451b27576a9142834725421aa6f0dd62ef3f71ca88cb72c4ef3c98868ac'
           

Scriptobject=Script.from_raw(scriptHEX)


print('\nDecode script:', Scriptobject.get_script())
