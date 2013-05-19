An additive synth for 64k intros.

Check notebook for details.


will sound like this https://play.spotify.com/track/6T7GevJq0BuPAr9h6ynmmX ("Postcards from far away")
http://www.pouet.net/prod.php?which=54089 (Namesia by Komplex)

k‰‰nnet‰‰n staattiseksi libiksi lopullista introa varten k‰ytˆn helpottamiseksi? 
http://msdn.microsoft.com/en-us/library/vstudio/ms235627.aspx#createlibproject


https://play.spotify.com/track/4eyZlJ2EKby3sF3KXwEDuP (Isao Tomita - Suite Bergamansque: Calir de Lune, No. 3)
lopun tremolopiano

joku helppo gabber-soundi: https://soundcloud.com/nextcyclone/the-melodyst-you-dont-know

Signal sources:
	envelope generators
	LFO's
	note pitch
	note velocity
	(other channel's output)
		--> sidechain
		
TODO:		

signaalin clamppaus process_voice_modulation funktioon?
FIKSAA voicep->envstate.volume saa outoja arvoja (v‰lin 0-1 ulkopuolelta)
	laita nuottien volume jonnekin fiksusti talteen ett‰ sit‰ voi kans k‰ytt‰‰ki
	hankkiudu mahdollisesti eroon envstaten volume-kent‰st‰